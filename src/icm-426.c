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


ICM426_Status_t ICM426_Init(ICM426_HandlerTypeDef *icmh, void *i2cDev_p, uint8_t devAddress)
{
  memset(icmh, 0, sizeof(ICM426_HandlerTypeDef));

  icmh->dev.i2c_p = i2cDev_p;
  icmh->dev.address = devAddress;

  return ICM426_OK;
}

ICM426_Status_t ICM426_InitFIFO(ICM426_HandlerTypeDef *icmh, uint8_t *buffer, uint16_t bufferSize)
{
  return ICM426_FIFO_Init(&icmh->FIFO, icmh, buffer, bufferSize);
}

ICM426_Status_t ICM426_Start(ICM426_HandlerTypeDef *icmh, ICM426_Config_t *config)
{
  ICM426_Status_t status;

  if (icmh->dev.i2c_p == 0) return ICM426_ERROR;

  // check connection
  if ((status = ICM426_CheckConnection(icmh)) != ICM426_OK) return status;


  ICM426_SetConfiguration(icmh, config);

  /**
   * 1. configuration
   *   - disable APEX feature | APEX_DISABLE in register SENSOR_CONFIG3
   * 2. wait clock
   * 3. power on 
   */

  ICM426_SwitchMode(icmh, ICM_PWR_ACCEL_LN_MODE|ICM_PWR_GYRO_LN|ICM_PWR_RC_OCILATOR_ON);

  return ICM426_OK;
}


ICM426_Status_t ICM426_SetConfiguration(ICM426_HandlerTypeDef *icmh, ICM426_Config_t *config)
{
  uint8_t config0;

  if (config)
    memcpy(&icmh->config, config, sizeof(ICM426_Config_t));

  // set scale
  if (config->accelODR == 0) config->accelODR = ICM_ODR_800_Hz;
  config0 = (config->accelScale & ICM_SCALE_ACCEL_MASK) | (config->accelODR & ICM_ODR_MASK);
  if (ICM426_I2C_WriteByte(&icmh->dev, ICM_R_ACCEL_CONFIG0, config0) != ICM426_OK)
    return ICM426_ERROR;

  if (config->gyroODR == 0) config->gyroODR = ICM_ODR_800_Hz;
  config0 = (config->gyroScale & ICM_SCALE_GYRO_MASK) | (config->gyroODR & ICM_ODR_MASK);
  if (ICM426_I2C_WriteByte(&icmh->dev, ICM_R_GYRO_CONFIG0, config0) != ICM426_OK)
    return ICM426_ERROR;

  return ICM426_OK;
}


ICM426_Status_t ICM426_SwitchMode(ICM426_HandlerTypeDef *icmh, uint8_t powerMode)
{
  ICM426_Status_t status = ICM426_ERROR;
  status = ICM426_I2C_WriteByte(&icmh->dev, ICM_R_PWR_MGMT0, powerMode);
  ICM426_Delay(2);
  return status;
}


ICM426_Status_t ICM426_CheckConnection(ICM426_HandlerTypeDef *icmh)
{
  ICM426_Status_t status;
  if ((status = ICM426_I2C_Probe(&icmh->dev)) != ICM426_OK)
    return status;

  if ((status = ICM426_I2C_ReadByte(&icmh->dev, (uint8_t) ICM_R_WHO_AM_I, &icmh->rxBuffer[0])) != ICM426_OK)
    return status;

  if (icmh->rxBuffer[0] != WHO_AM_I) return ICM426_ERROR;

  return ICM426_OK;
}


ICM426_Status_t ICM426_GetAccelerometer(ICM426_HandlerTypeDef *icmh, Vector_t *accel)
{
  ICM426_Status_t status;
  status = ICM426_I2C_Read(&icmh->dev, ICM_R_ACCEL_DATA_X1, &icmh->rxBuffer[0], 6);
  if (status != ICM426_OK) return status;

  READ_RAW_DATA_BIGENDIAN(&accel->x, &icmh->rxBuffer[0]);
  READ_RAW_DATA_BIGENDIAN(&accel->y, &icmh->rxBuffer[2]);
  READ_RAW_DATA_BIGENDIAN(&accel->z, &icmh->rxBuffer[4]);

  return ICM426_OK;
}


ICM426_Status_t ICM426_GetGyroscope(ICM426_HandlerTypeDef *icmh, Vector_t *gyro)
{
  ICM426_Status_t status;
  status = ICM426_I2C_Read(&icmh->dev, ICM_R_GYRO_DATA_X1, &icmh->rxBuffer[0], 6);
  if (status != ICM426_OK) return status;

  READ_RAW_DATA_BIGENDIAN(&gyro->x, &icmh->rxBuffer[0]);
  READ_RAW_DATA_BIGENDIAN(&gyro->y, &icmh->rxBuffer[2]);
  READ_RAW_DATA_BIGENDIAN(&gyro->z, &icmh->rxBuffer[4]);

  return ICM426_OK;
}


ICM426_Status_t ICM426_GetTemperature(ICM426_HandlerTypeDef *icmh, int16_t *temp)
{
  ICM426_Status_t status;
  status = ICM426_I2C_Read(&icmh->dev, ICM_R_TEMP_DATA1, &icmh->rxBuffer[0], 2);
  if (status != ICM426_OK) return status;

  READ_RAW_DATA_BIGENDIAN(temp, &icmh->rxBuffer[0]);

  return ICM426_OK;
}


ICM426_Status_t ICM426_Get6Axis(ICM426_HandlerTypeDef *icmh, Vector_t *accel, Vector_t *gyro)
{
  ICM426_Status_t status;
  status = ICM426_I2C_Read(&icmh->dev, ICM_R_ACCEL_DATA_X1, &icmh->rxBuffer[0], 12);
//  status = ICM426_I2C_Read(&icmh->dev, ICM_R_GYRO_DATA_X1, &icmh->rxBuffer[6], 6);
  if (status != ICM426_OK) return status;

  READ_RAW_DATA_BIGENDIAN(&accel->x, &icmh->rxBuffer[0]);
  READ_RAW_DATA_BIGENDIAN(&accel->y, &icmh->rxBuffer[2]);
  READ_RAW_DATA_BIGENDIAN(&accel->z, &icmh->rxBuffer[4]);
  READ_RAW_DATA_BIGENDIAN(&gyro->x, &icmh->rxBuffer[6]);
  READ_RAW_DATA_BIGENDIAN(&gyro->y, &icmh->rxBuffer[7]);
  READ_RAW_DATA_BIGENDIAN(&gyro->z, &icmh->rxBuffer[8]);

  return ICM426_OK;
}


