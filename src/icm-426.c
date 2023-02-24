/*
 * icm-426.c
 *
 *  Created on: Jul 26, 2022
 *      Author: janoko
 */

#include "include/icm-426.h"
#include "include/icm-426/utils.h"
#include "include/icm-i2c-dev.h"
#include <string.h>

#define WHO_AM_I 0x67


ICM426_Status_t ICM426_Init(ICM426_HandlerTypeDef *icmh,
                            ICM426_Config_t *config)
{
  memset(icmh, 0, sizeof(ICM426_HandlerTypeDef));

  // init fofp
  icmh->FIFO.icmh = icmh;

  return ICM426_OK;
}


ICM426_Status_t ICM426_Start(ICM426_HandlerTypeDef *icmh)
{
  ICM426_Status_t status;
  uint8_t isClockReady;

  if (icmh->i2c.check == 0 ||
      icmh->i2c.read  == 0 ||
      icmh->i2c.write == 0 ||
      icmh->delay == 0)
  {
    return ICM426_ERROR;
  }

  // check connection
  if ((status = ICM426_CheckConnection(icmh)) != ICM426_OK) return status;


  ICM426_SetConfiguration(icmh);

  /**
   * 1. configuration
   *   - disable APEX feature | APEX_DISABLE in register SENSOR_CONFIG3
   * 2. wait clock
   * 3. power on 
   */

  ICM426_SwitchMode(icmh, ICM_PWR_ACCEL_LN_MODE|ICM_PWR_GYRO_LN|ICM_PWR_RC_OCILATOR_ON);

checkClock:
  status = icmh->i2c.read((uint8_t) ICM_R_MCLK_RDY, &isClockReady, 1);
  if (status != ICM426_OK) return status;

  if ((isClockReady&0x8) == 0) {
    icmh->delay(2);
    goto checkClock;
  }
  return ICM426_OK;
}


ICM426_Status_t ICM426_SetConfiguration(ICM426_HandlerTypeDef *icmh)
{
  uint8_t config0;
  ICM426_Status_t status;

  // set data format as little endian
  status = ICM426_REG_WriteByte(icmh, ICM_R_INTF_CONFIG0,
                                ICM_B_FIFO_COUNT_FORMAT & ~(ICM_B_FIFO_COUNT_BIG_ENDIAN|
                                                            ICM_B_SENSOR_DATA_BIG_ENDIAN));
  if (status != ICM426_OK) return status;

  // set scale
  if (icmh->config.accelODR == 0) icmh->config.accelODR = ICM_ODR_800_Hz;
  config0 = (icmh->config.accelScale & ICM_SCALE_ACCEL_MASK) |
            (icmh->config.accelODR & ICM_ODR_MASK);
  status = ICM426_REG_WriteByte(icmh, ICM_R_ACCEL_CONFIG0, config0);
  if (status != ICM426_OK) return status;

  switch (icmh->config.accelScale) {
  case ICM_SCALE_ACCEL_16g:
    icmh->scaleFactor.accel = 16.0/32768.0;
    break;

  case ICM_SCALE_ACCEL_8g:
    icmh->scaleFactor.accel = 8.0/32768.0;
    break;

  case ICM_SCALE_ACCEL_4g:
    icmh->scaleFactor.accel = 4.0/32768.0;
    break;

  case ICM_SCALE_ACCEL_2g:
    icmh->scaleFactor.accel = 2.0/32768.0;
    break;
  }

  if (icmh->config.gyroODR == 0) icmh->config.gyroODR = ICM_ODR_800_Hz;
  config0 = (icmh->config.gyroScale & ICM_SCALE_GYRO_MASK) |
            (icmh->config.gyroODR & ICM_ODR_MASK);
  status = ICM426_REG_WriteByte(icmh, ICM_R_GYRO_CONFIG0, config0);
  if (status != ICM426_OK) return status;

  switch (icmh->config.gyroScale) {
  case ICM_SCALE_GYRO_2000dps:
    icmh->scaleFactor.gyro = 2000.0/32768.0;
    break;

  case ICM_SCALE_GYRO_1000dps:
    icmh->scaleFactor.gyro = 1000.0/32768.0;
    break;

  case ICM_SCALE_GYRO_500dps:
    icmh->scaleFactor.gyro = 500.0/32768.0;
    break;

  case ICM_SCALE_GYRO_250dps:
    icmh->scaleFactor.gyro = 250.0/32768.0;
    break;
  }

  return ICM426_OK;
}


ICM426_Status_t ICM426_SwitchMode(ICM426_HandlerTypeDef *icmh, uint8_t powerMode)
{
  ICM426_Status_t status = ICM426_ERROR;

  status = ICM426_REG_WriteByte(icmh, ICM_R_PWR_MGMT0, powerMode);
  if (status != ICM426_OK) return status;

  icmh->delay(2);
  return status;
}


ICM426_Status_t ICM426_CheckConnection(ICM426_HandlerTypeDef *icmh)
{
  ICM426_Status_t status;
  uint8_t whoAmIByte;

  status = icmh->i2c.check();
  if (status != ICM426_OK) return status;

  status = ICM426_REG_ReadByte(icmh, ICM_R_WHO_AM_I, &whoAmIByte);
  if (status != ICM426_OK) return status;

  if (whoAmIByte != WHO_AM_I) return ICM426_ERROR;

  return ICM426_OK;
}


ICM426_Status_t ICM426_GetAccelerometer(ICM426_HandlerTypeDef *icmh, ICM426_Vector_t *accel)
{
  ICM426_Status_t status;
  ICM426_RawVector_t tmp;

  status = icmh->i2c.read(ICM_R_ACCEL_DATA_X1, (uint8_t*)&tmp, 6);
  if (status != ICM426_OK) return status;

  accel->x = ((float) tmp.x)*icmh->scaleFactor.accel;
  accel->y = ((float) tmp.y)*icmh->scaleFactor.accel;
  accel->z = ((float) tmp.z)*icmh->scaleFactor.accel;

  return ICM426_OK;
}


ICM426_Status_t ICM426_GetGyroscope(ICM426_HandlerTypeDef *icmh, ICM426_Vector_t *gyro)
{
  ICM426_Status_t status;
  ICM426_RawVector_t tmp;

  status = icmh->i2c.read(ICM_R_GYRO_DATA_X1, (uint8_t*)&tmp, 6);
  if (status != ICM426_OK) return status;

  gyro->x = ((float) tmp.x)*icmh->scaleFactor.gyro;
  gyro->y = ((float) tmp.y)*icmh->scaleFactor.gyro;
  gyro->z = ((float) tmp.z)*icmh->scaleFactor.gyro;

  return ICM426_OK;
}


ICM426_Status_t ICM426_GetTemperature(ICM426_HandlerTypeDef *icmh, int16_t *temp)
{
  ICM426_Status_t status;
  status = icmh->i2c.read(ICM_R_TEMP_DATA1, (uint8_t*) temp, 2);
  if (status != ICM426_OK) return status;

  return ICM426_OK;
}


ICM426_Status_t ICM426_Get6Axis(ICM426_HandlerTypeDef *icmh, ICM426_Vector_t *accel, ICM426_Vector_t *gyro)
{
  ICM426_Status_t status;
  ICM426_RawVector_t tmp[2];

  status = icmh->i2c.read(ICM_R_ACCEL_DATA_X1, (uint8_t*) &tmp[0], 12);
  if (status != ICM426_OK) return status;

  accel->x = ((float) tmp[0].x)*icmh->scaleFactor.accel;
  accel->y = ((float) tmp[0].y)*icmh->scaleFactor.accel;
  accel->z = ((float) tmp[0].z)*icmh->scaleFactor.accel;

  gyro->x = ((float) tmp[1].x)*icmh->scaleFactor.gyro;
  gyro->y = ((float) tmp[1].y)*icmh->scaleFactor.gyro;
  gyro->z = ((float) tmp[1].z)*icmh->scaleFactor.gyro;

  return ICM426_OK;
}


