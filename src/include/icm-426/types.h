/*
 * types.h
 *
 *  Created on: Jul 26, 2022
 *      Author: janoko
 */

#ifndef DRIVERS_ICM_426_TYPES_H_
#define DRIVERS_ICM_426_TYPES_H_

#include <stdint.h>

typedef struct {
  int16_t x;
  int16_t y;
  int16_t z;
} ICM426_RawVector_t;

typedef struct {
  float x;
  float y;
  float z;
} ICM426_Vector_t;

typedef enum {
  ICM426_OK,
  ICM426_ERROR,
  ICM426_DEV_BUSY,
  ICM426_TIMEOUT,
} ICM426_Status_t;


#endif /* DRIVERS_ICM_426_SRC_INCLUDE_TYPES_H_ */
