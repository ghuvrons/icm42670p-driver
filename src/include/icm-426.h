/*
 * icm-426.h
 *
 *  Created on: Jul 26, 2022
 *      Author: janoko
 */

#ifndef DRIVER_ICM_426_H_
#define DRIVER_ICM_426_H_

#include "icm-426/types.h"
#include "icm-426/register.h"
#include "icm-426/fifo.h"
#include "icm-i2c-dev.h"

#define ICM_SCALE_ACCEL_MASK    0x60
#define ICM_SCALE_ACCEL_16g     0x00
#define ICM_SCALE_ACCEL_8g      0x20
#define ICM_SCALE_ACCEL_4g      0x40
#define ICM_SCALE_ACCEL_2g      0x60

#define ICM_SCALE_GYRO_MASK     0x60
#define ICM_SCALE_GYRO_2000dps  0x00
#define ICM_SCALE_GYRO_1000dps  0x20
#define ICM_SCALE_GYRO_500dps   0x40
#define ICM_SCALE_GYRO_250dps   0x60

#define ICM_ODR_MASK            0x0F
#define ICM_ODR_1_6k_Hz         0x05
#define ICM_ODR_800_Hz          0x06
#define ICM_ODR_400_Hz          0x07
#define ICM_ODR_200_Hz          0x08
#define ICM_ODR_100_Hz          0x09
#define ICM_ODR_50_Hz           0x0A
#define ICM_ODR_25_Hz           0x0B
#define ICM_ODR_12_5_Hz         0x0C
#define ICM_ODR_6_25_Hz         0x0D
#define ICM_ODR_3_125_Hz        0x0E
#define ICM_ODR_1_5625_Hz       0x0F

#define ICM_PWR_ACCELL_MASK     0x83
#define ICM_PWR_ACCEL_OFF       0x00
#define ICM_PWR_ACCEL_LP_MODE   0x02
#define ICM_PWR_ACCEL_LN_MODE   0x83
#define ICM_PWR_GYRO_MASK       0x0C
#define ICM_PWR_GYRO_OFF        0x00
#define ICM_PWR_GYRO_STDBY      0x04
#define ICM_PWR_GYRO_LN         0x0C
#define ICM_PWR_RC_OCILATOR_ON  0x10


#define ICM426_STATUS_CONNECTED   0x01
#define ICM426_STATUS_CALIBRATED  0x02

#define ICM426_RX_BUF_SZ 24
#define ICM426_TX_BUF_SZ 8

typedef struct {
  /*
   * scale
   * interrupt enable
   */
  uint8_t accelScale;
  uint8_t accelODR;
  uint8_t gyroScale;
  uint8_t gyroODR;

} ICM426_Config_t;

typedef struct {
  uint8_t status;
  ICM426_Config_t config;

  struct {
    ICM426_Status_t (*check)(void);
    ICM426_Status_t (*read)(uint8_t regAddr, uint8_t *data, uint8_t length);
    ICM426_Status_t (*write)(uint8_t regAddr, uint8_t *data, uint8_t length);
  } i2c;

  void (*delay)(uint32_t ms);

  struct {
    float accel;
    float gyro;
  } scaleFactor;

  // FIFO reader
  ICM426_FIFO_t FIFO;
} ICM426_HandlerTypeDef;


ICM426_Status_t ICM426_Init(ICM426_HandlerTypeDef*, ICM426_Config_t*);
ICM426_Status_t ICM426_Start(ICM426_HandlerTypeDef*);
ICM426_Status_t ICM426_SetConfiguration(ICM426_HandlerTypeDef*);
ICM426_Status_t ICM426_SwitchMode(ICM426_HandlerTypeDef*, uint8_t);
ICM426_Status_t ICM426_CheckConnection(ICM426_HandlerTypeDef*);

ICM426_Status_t ICM426_GetAccelerometer(ICM426_HandlerTypeDef*, ICM426_Vector_t *accel);
ICM426_Status_t ICM426_GetGyroscope(ICM426_HandlerTypeDef*, ICM426_Vector_t *gyro);
ICM426_Status_t ICM426_GetTemperature(ICM426_HandlerTypeDef*, int16_t *temp);
ICM426_Status_t ICM426_Get6Axis(ICM426_HandlerTypeDef*, ICM426_Vector_t *accel, ICM426_Vector_t *gyro);

// I2C

#endif /* DRIVER_ICM_426_H_ */
