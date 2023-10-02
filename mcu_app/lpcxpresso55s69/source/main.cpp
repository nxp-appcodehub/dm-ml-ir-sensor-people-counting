/*
 * Copyright 2023 NXP
 * All rights reserved.
 */

#include <stdio.h>
#include "board.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "fsl_debug_console.h"
#include "fsl_ctimer.h"
#include "fsl_power.h"
#include "amg88xx.h"
#include "fsl_gpio.h"

#include "timer.h"
#include "model_proc.h"
#include "output_postproc.h"
/*******************************************************************************
 * Defines
 ******************************************************************************/
//#define APP_COLLECT_DATA
#define APP_SAMPLING_RATE 10

#define CTIMER             CTIMER4
#define CTIMER_CLK         kFRO_HF_to_CTIMER4
#define CTIMER_MAT_OUT     kCTIMER_Match_0
#define CTIMER_IRQ_HANDLER CTIMER4_IRQHandler
#define CTIMER_START()     (CTIMER->TCR |= CTIMER_TCR_CEN_MASK)
#define CTIMER_STOP()      (CTIMER->TCR &= ~CTIMER_TCR_CEN_MASK)

#define LED_GPIO       BOARD_LED_BLUE_GPIO
#define LED_PORT       BOARD_LED_BLUE_GPIO_PORT
#define LED_PIN        BOARD_LED_BLUE_GPIO_PIN
#define LED_CLOCK      kCLOCK_Gpio1
#define LED_ON()       GPIO_PinWrite(LED_GPIO, LED_PORT, LED_PIN, 0)
#define LED_OFF()      GPIO_PinWrite(LED_GPIO, LED_PORT, LED_PIN, 1)

/*******************************************************************************
 * Variables
 ******************************************************************************/
volatile struct app_state_t {
    uint32_t num_samples;
    uint32_t target_samples;
    bool data_ready;
    bool running;
} app_state;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
static void CTimerConfig(int sampling_rate_hz, bool periodic_en);
static void LedInit(void);
static void GridEyeInit(void);
static uint32_t GetTargetSamples(void);
#if defined(APP_COLLECT_DATA)
static void LogData(void);
#else
static void RunModel(void);
#endif
/*******************************************************************************
 * Interrupt Handlers
 ******************************************************************************/
extern "C" {
void CTIMER_IRQ_HANDLER(void) {
    CTIMER->IR |= CTIMER->IR;
    app_state.data_ready = true;
    SDK_ISR_EXIT_BARRIER;
}
}
/*******************************************************************************
 * API
 ******************************************************************************/
int main(void) {
    POWER_SetBodVbatLevel(kPOWER_BodVbatLevel1650mv, kPOWER_BodHystLevel50mv, false);
    CLOCK_AttachClk(BOARD_DEBUG_UART_CLK_ATTACH);

    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitDebugConsole();

    GridEyeInit();
    LedInit();

#if !defined(APP_COLLECT_DATA)
    if (MODEL_Init() != kStatus_Success) {
        PRINTF("Failed initializing model" "\r\n");
        for (;;) {}
    }
#endif

    while (1) {
        app_state.data_ready = false;
        app_state.running = false;
        app_state.num_samples = 0;
        app_state.target_samples = GetTargetSamples();

        CTimerConfig(APP_SAMPLING_RATE, true);
        LED_ON();
        CTIMER_START();

#if defined(APP_COLLECT_DATA)
        LogData();
#else
        RunModel();
#endif

        CTIMER_STOP();
        LED_OFF();
    }

    return 0;
}

static void CTimerConfig(int sampling_rate_hz, bool periodic_en) {
    ctimer_config_t config;
    ctimer_match_config_t matchConfig;

    CLOCK_AttachClk(CTIMER_CLK);
    CTIMER_GetDefaultConfig(&config);
    CTIMER_Init(CTIMER, &config);

    matchConfig.enableCounterReset = true;
    matchConfig.enableCounterStop  = !periodic_en;
    matchConfig.matchValue         = CLOCK_GetCTimerClkFreq(4U)/sampling_rate_hz;
    matchConfig.outControl         = kCTIMER_Output_NoAction;
    matchConfig.outPinInitState    = false;
    matchConfig.enableInterrupt    = true;
    CTIMER_SetupMatch(CTIMER, CTIMER_MAT_OUT, &matchConfig);
}


static void LedInit(void) {
    CLOCK_EnableClock(LED_CLOCK);

    gpio_pin_config_t sw_config = {
        kGPIO_DigitalOutput,
        1,
    };

    GPIO_PinInit(LED_GPIO, LED_PORT, LED_PIN, &sw_config);
}

static void GridEyeInit(void) {
    /* Pins are initialized in  BOARD_InitBootPins*/
    CLOCK_AttachClk(kFRO12M_to_FLEXCOMM4);
    RESET_PeripheralReset(kFC4_RST_SHIFT_RSTn);

    amg88xx_config_t ir_config;
    AMG88XX_GetDefaultConfig(&ir_config);
    ir_config.i2c_id = 4;
    ir_config.src_clk_hz = CLOCK_GetFro12MFreq();
    if (AMG88XX_Init(&ir_config) != kStatus_Success) {
        PRINTF("Failed to init sensor!\r\n");
        while(1);
    }
}

static uint32_t GetTargetSamples(void) {
    uint32_t target_samples;

    // Synch with script
    PRINTF("Press ENTER to begin\r\n");
    GETCHAR();

    PRINTF("Number of Samples (0 for continuous): ");
    SCANF((char *)"%d", &target_samples);
    PRINTF("%d\n", target_samples);

    // Wait for the start signal
//    GETCHAR();

    return target_samples;
}

#if defined(APP_COLLECT_DATA)
static void LogData(void) {
    status_t status;
    float thermistor_temp;
    float grid[AMG88XX_GetGridSize()];

    app_state.running = true;
    while (app_state.running) {
        while (!app_state.data_ready);
        app_state.data_ready = false;

        status = AMG88XX_ReadGridCelsius(grid);
        if (status != kStatus_Success) {
            continue;
        }

        status = AMG88XX_ReadThermistor(&thermistor_temp);
        if (status != kStatus_Success) {
            continue;
        }

        PRINTF("%f,",thermistor_temp);
        for (uint32_t i = 0; i < AMG88XX_GetGridSize(); ++i) {
            PRINTF("%f,", grid[i]);
        }
        PRINTF("\n");

        if ((app_state.target_samples > 0) &&
            (++app_state.num_samples == app_state.target_samples)) {
            app_state.running = false;
        }
    }
}
#else
static void RunModel(void) {
    tensor_dims_t inputDims;
    tensor_type_t inputType;
    uint8_t* inputData = MODEL_GetInputTensorData(&inputDims, &inputType);

    tensor_dims_t outputDims;
    tensor_type_t outputType;
    uint8_t* outputData = MODEL_GetOutputTensorData(&outputDims, &outputType);

    app_state.running = true;
    while (app_state.running) {
        while (!app_state.data_ready);
        app_state.data_ready = false;

        if (AMG88XX_ReadGridCelsius((float *)inputData) != kStatus_Success) {
            continue;
        }

        auto startTime = TIMER_GetTimeInUS();
        MODEL_RunInference();
        auto endTime = TIMER_GetTimeInUS();

        MODEL_ProcessOutput(outputData, &outputDims, outputType, endTime - startTime);

        if ((app_state.target_samples > 0) &&
            (++app_state.num_samples == app_state.target_samples)) {
            app_state.running = false;
        }
    }
}
#endif
