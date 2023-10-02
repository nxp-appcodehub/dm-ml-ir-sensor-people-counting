#ifndef _AMG88XX_DRV_H_
#define AMG88XX_DRV_H_
/*
 * Copyright 2023
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_common.h"
#include "fsl_adapter_i2c.h"

typedef struct _config {
    uint8_t i2c_id;
    uint32_t baudrate_hz;
    uint32_t src_clk_hz;
    uint8_t address;
} amg88xx_config_t;

typedef void(*int_cb_t)(void);

#if defined(__cplusplus)
extern "C" {
#endif
void AMG88XX_GetDefaultConfig(amg88xx_config_t *config);
/*
 * Initializes I2C and configures the sensor
 */
status_t AMG88XX_Init(amg88xx_config_t *config);

/*
 * Reads a word form the desired register
 */
status_t AMG88XX_Read(uint8_t reg, uint8_t *data, uint32_t len);

/*
 * Writes byte to desired register
 */
status_t AMG88XX_Write(uint8_t reg, uint8_t *data, uint32_t len);
status_t AMG88XX_WriteByte(uint8_t reg, uint8_t data);

/*
 * Reads the grid as float array
 */
status_t AMG88XX_ReadGridCelsius(float *grid);

/*
 * Reads the raw values into the specified buffer.
 */
status_t AMG88XX_ReadGridRaw(int16_t *grid);

/*
 * Read thermistor value
 */
status_t AMG88XX_ReadThermistor(float *value);

/*
 * Gets the specified values.
 */
const uint16_t AMG88XX_GetGridSize(void);
const uint8_t AMG88XX_GetGridNumRows(void);
const uint8_t AMG88XX_GetGridNumCols(void);
const float AMG88XX_GetTempCoeff(void);

#if defined(__cplusplus)
}
#endif
#endif /*_AMG88XX_DRV_H_*/
