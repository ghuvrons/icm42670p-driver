/*
 * utils.h
 *
 *  Created on: Jul 28, 2022
 *      Author: janoko
 */

#ifndef DRIVERS_ICM_426_UTILS_H_
#define DRIVERS_ICM_426_UTILS_H_

#include "types.h"
#include "../icm-i2c-dev.h"

ICM426_Status_t ICM426_REG_ReadByte(ICM426_HandlerTypeDef*, uint16_t regAddr, uint8_t *byte);
ICM426_Status_t ICM426_REG_WriteByte(ICM426_HandlerTypeDef*, uint16_t regAddr, uint8_t byte);

#endif /* DRIVERS_ICM_426_SRC_INCLUDE_ICM_426_UTILS_H_ */
