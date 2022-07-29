/*
 * icm-i2c-dev.c
 *
 *  Created on: Jul 26, 2022
 *      Author: janoko
 */

#include "include/icm-i2c-dev.h"
#include "include/icm-426/register.h"


__attribute__((weak)) ICM426_Status_t ICM426_I2C_Probe(ICM426_Dev_t *i2cDev_p)
{
  return ICM426_OK;
}


__attribute__((weak)) ICM426_Status_t ICM426_I2C_Read(ICM426_Dev_t *i2cDev_p, uint8_t regAddr, uint8_t *data, uint8_t length)
{
  return ICM426_OK;
}


__attribute__((weak)) ICM426_Status_t ICM426_I2C_Write(ICM426_Dev_t *i2cDev_p, uint8_t regAddr, uint8_t *data, uint8_t length)
{
  return ICM426_OK;
}


ICM426_Status_t ICM426_I2C_WriteByte(ICM426_Dev_t *i2cDev_p, uint8_t regAddr, uint8_t byte)
{
  return ICM426_I2C_Write(i2cDev_p, regAddr, &byte, 1);
}
