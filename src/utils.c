/*
 * utils.c
 *
 *  Created on: Jul 28, 2022
 *      Author: janoko
 */

#include "include/icm-426.h"
#include "include/icm-i2c-dev.h"
#include "include/icm-426/register.h"
#include "include/icm-426/utils.h"


__attribute__((weak)) void ICM426_Delay(uint32_t ms) {}\


ICM426_Status_t ICM426_MREG_ReadByte(ICM426_Dev_t *i2cDev_p, uint16_t regAddr, uint8_t *byte)
{
  ICM426_I2C_WriteByte(i2cDev_p, ICM_R_BLK_SEL_R, (regAddr & 0xFE00) >> 8);
  ICM426_I2C_WriteByte(i2cDev_p, ICM_R_MADDR_R, (regAddr & 0x00FF));
  ICM426_Delay(1);
  ICM426_I2C_ReadByte(i2cDev_p, ICM_R_M_R, byte);
  ICM426_Delay(1);
  ICM426_I2C_WriteByte(i2cDev_p, ICM_R_BLK_SEL_R, 0x00);
  return ICM426_OK;
}


ICM426_Status_t ICM426_MREG_WriteByte(ICM426_Dev_t *i2cDev_p, uint16_t regAddr, uint8_t byte)
{
  ICM426_I2C_WriteByte(i2cDev_p, ICM_R_BLK_SEL_W, (regAddr & 0xFE00) >> 8);
  ICM426_I2C_WriteByte(i2cDev_p, ICM_R_MADDR_W, (regAddr & 0x00FF));
  ICM426_I2C_WriteByte(i2cDev_p, ICM_R_M_W, byte);
  ICM426_Delay(1);
  ICM426_I2C_WriteByte(i2cDev_p, ICM_R_BLK_SEL_W, 0x00);
  return ICM426_OK;
}
