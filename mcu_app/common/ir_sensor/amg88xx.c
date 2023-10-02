/*
 * Copyright 2023
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "amg88xx.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define CHECK_ERROR(stat) {if (stat != kStatus_Success) {return stat;}}

#define GRID_START_REG    0x80
#define NUM_GPIO_CHANNELS (kGPIO_InterruptOutput1 + 1)
typedef struct _device_ctx {
	amg88xx_config_t cfg;
    HAL_I2C_MASTER_HANDLE_DEFINE(i2c_master_handle);

    const uint8_t grid_size;
    const uint8_t grid_rows;
    const uint8_t grid_cols;
    const float temp_coeff;
    const float thermistor_coeff;
} device_ctx_t;

/*******************************************************************************
 * Variables
 ******************************************************************************/
static device_ctx_t dev = {
    .grid_size  = 64u,
    .grid_rows  = 8u,
    .grid_cols  = 8u,
    .temp_coeff = 0.25,
    .thermistor_coeff = 0.0625
};

/*******************************************************************************
 * Code
 ******************************************************************************/
void AMG88XX_GetDefaultConfig(amg88xx_config_t *config) {
	config->i2c_id = 0;
    config->baudrate_hz = 10000;
    config->src_clk_hz = 0;
    config->address = 0x68;
}

status_t AMG88XX_Init(amg88xx_config_t *config) {
	 hal_i2c_master_config_t master_config;

	 assert(config->src_clk_hz != 0);
	 dev.cfg = *config;

	 master_config.enableMaster   = true;
	 master_config.baudRate_Bps   = config->baudrate_hz;
	 master_config.srcClock_Hz    = config->src_clk_hz;
	 master_config.instance       = config->i2c_id;
	 HAL_I2cMasterInit((hal_i2c_master_handle_t)dev.i2c_master_handle, &master_config);

    /* Init sensor */
    CHECK_ERROR(AMG88XX_WriteByte(0x00, 0x00)); // Normal mode
    CHECK_ERROR(AMG88XX_WriteByte(0x01, 0x3F)); // Initial reset
    CHECK_ERROR(AMG88XX_WriteByte(0x02, 0x00)); // Frame mode 10FPS
    CHECK_ERROR(AMG88XX_WriteByte(0x08, 0x30)); // INTHL
    CHECK_ERROR(AMG88XX_WriteByte(0x09, 0x00)); // INTHH
    CHECK_ERROR(AMG88XX_WriteByte(0x0A, 0x15)); // INTLL
    CHECK_ERROR(AMG88XX_WriteByte(0x0B, 0x00)); // INTLH
    CHECK_ERROR(AMG88XX_WriteByte(0x0C, 0x20)); // IHSYL
    CHECK_ERROR(AMG88XX_WriteByte(0x0D, 0x00)); // IHYSH
    // Enable moving average
    CHECK_ERROR(AMG88XX_WriteByte(0x1F, 0x50));
    CHECK_ERROR(AMG88XX_WriteByte(0x1F, 0x45));
    CHECK_ERROR(AMG88XX_WriteByte(0x1F, 0x57));
    CHECK_ERROR(AMG88XX_WriteByte(0x07, 0x20));
    CHECK_ERROR(AMG88XX_WriteByte(0x1F, 0x00));

    return kStatus_Success;
}

status_t AMG88XX_Read(uint8_t reg, uint8_t *data, uint32_t len) {
	hal_i2c_master_transfer_t xfer;
    xfer.slaveAddress   = dev.cfg.address;
    xfer.direction      = kHAL_I2cRead;
    xfer.subaddress     = (uint32_t)reg;
    xfer.subaddressSize = 1;
    xfer.data           = data;
    xfer.dataSize       = len;
    xfer.flags          = kHAL_I2cTransferDefaultFlag;
    return HAL_I2cMasterTransferBlocking((hal_i2c_master_handle_t)dev.i2c_master_handle, &xfer);
}

status_t AMG88XX_Write(uint8_t reg, uint8_t *data, uint32_t len) {
	hal_i2c_master_transfer_t xfer;
    xfer.slaveAddress   = dev.cfg.address;
    xfer.direction      = kHAL_I2cWrite;
    xfer.subaddress     = (uint32_t)reg;
    xfer.subaddressSize = 1;
    xfer.data           = data;
    xfer.dataSize       = len;
    xfer.flags          = kHAL_I2cTransferDefaultFlag;
    return HAL_I2cMasterTransferBlocking((hal_i2c_master_handle_t)dev.i2c_master_handle, &xfer);
}

status_t AMG88XX_WriteByte(uint8_t reg, uint8_t data) {
	hal_i2c_master_transfer_t xfer;
    xfer.slaveAddress   = dev.cfg.address;
    xfer.direction      = kHAL_I2cWrite;
    xfer.subaddress     = (uint32_t)reg;
    xfer.subaddressSize = 1;
    xfer.data           = &data;
    xfer.dataSize       = 1;
    xfer.flags          = kHAL_I2cTransferDefaultFlag;
    return HAL_I2cMasterTransferBlocking((hal_i2c_master_handle_t)dev.i2c_master_handle, &xfer);
}

status_t AMG88XX_ReadGridCelsius(float *grid) {
    uint8_t i;
    int16_t raw[dev.grid_size];

    CHECK_ERROR(AMG88XX_ReadGridRaw(raw));

    for (i = 0; i < dev.grid_size; ++i) {
        grid[i] = (raw[i] * dev.temp_coeff);
    }

    return kStatus_Success;
}

status_t AMG88XX_ReadGridRaw(int16_t *grid) {
    CHECK_ERROR(AMG88XX_Read(GRID_START_REG,
            (uint8_t *)grid,
            dev.grid_size*sizeof(int16_t)));

    // sign extend
    for (uint32_t i = 0; i < dev.grid_size; ++i) {
        grid[i] = ((int16_t)(grid[i] << 4)) >> 4;
    }

    return kStatus_Success;
}

status_t AMG88XX_ReadThermistor(float *value) {
    int16_t raw = 0;
    CHECK_ERROR(AMG88XX_Read(0x0E,
            (uint8_t *)&raw,
            2));
    *value = (float)((raw << 4) >> 4) * dev.thermistor_coeff;
    return kStatus_Success;
}

const uint16_t AMG88XX_GetGridSize(void) {
    return dev.grid_size;
}

const uint8_t AMG88XX_GetGridNumRows(void) {
    return dev.grid_rows;
}

const uint8_t AMG88XX_GetGridNumCols(void) {
    return dev.grid_cols;
}

const float AMG88XX_GetTempCoeff(void) {
    return dev.temp_coeff;
}
