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

#define READ_RAW_DATA_BIGENDIAN(dst, src) {                     \
  *((uint16_t*) (dst)) = (*((uint8_t*) (src))) << 8;  \
  *((uint16_t*) (dst)) |= *(((uint8_t*) (src))+1);    \
}

void ICM426_Delay(uint32_t ms);

ICM426_Status_t ICM426_MREG_ReadByte(ICM426_Dev_t*, uint16_t regAddr, uint8_t *byte);
ICM426_Status_t ICM426_MREG_WriteByte(ICM426_Dev_t*, uint16_t regAddr, uint8_t byte);

#endif /* DRIVERS_ICM_426_SRC_INCLUDE_ICM_426_UTILS_H_ */
