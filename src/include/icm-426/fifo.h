/*
 * icm-fifo.h
 *
 *  Created on: Jul 27, 2022
 *      Author: janoko
 */

#ifndef DRIVERS_ICM_426_FIFO_H_
#define DRIVERS_ICM_426_FIFO_H_

#include "types.h"

// FIFO_CONFIG1
#define ICM_FIFO_CONFIG1_MASK         0x0003
#define ICM_FIFO_BYPASS               0x0001
#define ICM_FIFO_STOP_ON_FULL         0x0002

// FIFO_CONFIG5
// Shift 2 bit to right in icm reg
#define ICM_FIFO_CONFIG5_MASK         0x00FC
#define ICM_FIFO_ACCEL_EN             0x0004
#define ICM_FIFO_GYRO_EN              0x0008
#define ICM_FIFO_TMST_FSYNC_EN        0x0010
#define ICM_FIFO_HIRES_EN             0x0020
#define ICM_FIFO_RESUME_PARTIAL_RD    0x0048
#define ICM_FIFO_WM_GT_TH             0x0080

// FIFO_CONFIG6
// Shift 8 bit to right in icm reg
#define ICM_FIFO_CONFIG6_MASK         0x1100
#define ICM_RCOSC_REQ_ON_FIFO_THS_DIS 0x0100
#define ICM_FIFO_EMPTY_INDICATOR_DIS  0x1000

// Packet Header
#define ICM_P_HEADER_ODR_GYRO         0x01
#define ICM_P_HEADER_ODR_ACCEL        0x02
#define ICM_P_HEADER_TIMESTAMP_FSYNC  0x0C
#define ICM_P_HEADER_20               0x10
#define ICM_P_HEADER_GYRO             0x20
#define ICM_P_HEADER_ACCEL            0x40
#define ICM_P_HEADER_MSG              0x80


typedef struct {
  Vector_t gyro;
  Vector_t accel;
  float temperature;
  uint16_t timestamp;
} ICM426_Data_t;

typedef struct {
  void          *icmh;
  ICM426_Data_t *buffer;
  uint16_t      bufferSize;
  uint16_t      rPosData;
  uint16_t      wPosData;
  uint8_t       isWriteOverlap;
  uint8_t       writeOffset;
  uint16_t      lostData;

  struct {
    uint8_t dataRate; //  FDR_SEL in register FDR_CONFIG
    uint16_t feature_en;
  } config;

  struct {
    uint8_t header;
    struct { uint8_t x[2]; uint8_t y[2]; uint8_t z[2];} accel;
    struct { uint8_t x[2]; uint8_t y[2]; uint8_t z[2];} gyro;
    uint8_t temperature[2];
    uint8_t timestamp[2];
    uint8_t ext[3];
  } rawPacket;
} ICM426_FIFO_t;


ICM426_Status_t ICM426_FIFO_Init(ICM426_FIFO_t*, void *icmh, uint8_t *buffer, uint16_t bufferSize);
ICM426_Status_t ICM426_FIFO_Start(ICM426_FIFO_t*);
ICM426_Status_t ICM426_FIFO_FeatureEnable(ICM426_FIFO_t*, uint16_t features);
ICM426_Status_t ICM426_FIFO_FeatureDisable(ICM426_FIFO_t*, uint16_t features);
uint16_t        ICM426_FIFO_GetData(ICM426_FIFO_t*);
ICM426_Status_t ICM426_FIFO_Flush(ICM426_FIFO_t*);

#endif /* DRIVERS_ICM_426_FIFO_H_ */
