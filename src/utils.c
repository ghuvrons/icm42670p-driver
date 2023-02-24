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


ICM426_Status_t ICM426_REG_ReadByte(ICM426_HandlerTypeDef *icmh, uint16_t regAddr, uint8_t *byte)
{
  ICM426_Status_t status;
  uint8_t isClockReady = 0;
  uint8_t writeByte = 0;

  // if MREG0
  if ((regAddr & 0xFF00) == 0)
    return icmh->i2c.read((uint8_t) (regAddr & 0x00FF), byte, 1);

  status = icmh->i2c.read((uint8_t) ICM_R_MCLK_RDY, &isClockReady, 1);
  if (status != ICM426_OK || (isClockReady&0x8) == 0) return status;

  writeByte = (regAddr & 0xFE00) >> 8;
  status = icmh->i2c.write((uint8_t) ICM_R_BLK_SEL_R, &writeByte, 1);
  if (status != ICM426_OK) return status;

  writeByte = (regAddr & 0x00FF);
  status = icmh->i2c.write((uint8_t) ICM_R_MADDR_R, &writeByte, 1);
  if (status != ICM426_OK) return status;

  ICM426_Delay(1);

  status = icmh->i2c.read((uint8_t) ICM_R_M_R, byte, 1);
  if (status != ICM426_OK) return status;

  ICM426_Delay(1);

  return ICM426_OK;
}


ICM426_Status_t ICM426_REG_WriteByte(ICM426_HandlerTypeDef *icmh,
                                     uint16_t regAddr, uint8_t byte)
{
  ICM426_Status_t status;
  uint8_t isClockReady = 0;
  uint8_t writeByte = 0;

  // if MREG0
  if ((regAddr & 0xFF00) == 0)
    return icmh->i2c.write((uint8_t) (regAddr & 0x00FF), &byte, 1);

  status = icmh->i2c.read((uint8_t) ICM_R_MCLK_RDY, &isClockReady, 1);
  if (status != ICM426_OK || (isClockReady&0x8) == 0) return status;

  writeByte = (regAddr & 0xFE00) >> 8;
  status = icmh->i2c.write((uint8_t) ICM_R_BLK_SEL_W, &writeByte, 1);
  if (status != ICM426_OK) return status;

  writeByte = (regAddr & 0x00FF);
  status = icmh->i2c.write((uint8_t) ICM_R_MADDR_W, &writeByte, 1);
  if (status != ICM426_OK) return status;

  status = icmh->i2c.write((uint8_t) ICM_R_M_W, &byte, 1);
  if (status != ICM426_OK) return status;

  ICM426_Delay(1);

  return ICM426_OK;
}
