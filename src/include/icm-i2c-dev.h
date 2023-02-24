/*
 * icm-i2c-dev.h
 *
 *  Created on: Jul 26, 2022
 *      Author: janoko
 */

#ifndef DRIVERS_ICM_I2C_DEV_H_
#define DRIVERS_ICM_I2C_DEV_H_

#include "icm-426/types.h"

// MACROS
#define ICM426_I2C_ReadByte(i2ch, regAddr, data) (ICM426_I2C_Read((i2ch), (regAddr), (data), 1))


typedef struct {
  void *i2c_p;
  uint8_t address;
} ICM426_Dev_t;

ICM426_Status_t ICM426_I2C_WriteByte(ICM426_Dev_t*, uint8_t regAddr, uint8_t byte);

#endif /* DRIVERS_ICM_426_SRC_INCLUDE_ICM_I2C_DEV_H_ */
