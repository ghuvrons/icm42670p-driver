/*
 * icm-fifo.c
 *
 *  Created on: Jul 27, 2022
 *      Author: janoko
 */


#include "include/icm-426.h"
#include "include/icm-426/utils.h"
#include <string.h>



ICM426_Status_t ICM426_FIFO_Init(ICM426_FIFO_t *fifoPtr, void *icmh, uint8_t *buffer, uint16_t bufferSize)
{
  fifoPtr->icmh = icmh;
  fifoPtr->bufferSize = bufferSize / sizeof(ICM426_Data_t);
  if (fifoPtr->bufferSize < 2) {
    return ICM426_ERROR;
  }
  fifoPtr->buffer = (ICM426_Data_t*) buffer;
  return ICM426_OK;
}


ICM426_Status_t ICM426_FIFO_Start(ICM426_FIFO_t *fifoPtr)
{
  ICM426_HandlerTypeDef *icmh     = (ICM426_HandlerTypeDef*) fifoPtr->icmh;

  if (icmh == 0) return ICM426_ERROR;

  uint16_t fifoWatermark = 100 & 0x0FFF;
  uint8_t myB = 0;


  ICM426_I2C_WriteByte(&icmh->dev, ICM_R_FIFO_CONFIG1, 0x00);
  ICM426_I2C_WriteByte(&icmh->dev, ICM_R_FIFO_CONFIG2, fifoWatermark & 0x00FF);
  ICM426_I2C_WriteByte(&icmh->dev, ICM_R_FIFO_CONFIG3, fifoWatermark >> 8);
  ICM426_MREG_ReadByte(&icmh->dev, ICM_R_FDR_CONFIG, &myB);
  ICM426_MREG_ReadByte(&icmh->dev, ICM_R_FIFO_CONFIG5, &myB);
  ICM426_MREG_WriteByte(&icmh->dev, ICM_R_FIFO_CONFIG5, 0x03);
  ICM426_MREG_WriteByte(&icmh->dev, ICM_R_TMST_CONFIG1, 0x00);

  return ICM426_OK;
}


uint16_t ICM426_FIFO_GetData(ICM426_FIFO_t *fifoPtr)
{
  uint16_t              readLen   = 0;
  uint16_t              fifoCount = 0;
  ICM426_HandlerTypeDef *icmh     = (ICM426_HandlerTypeDef*) fifoPtr->icmh;
  uint8_t               *raw      = (uint8_t*) &fifoPtr->rawPacket;
  ICM426_Data_t         *dataPtr;

  if (fifoPtr->buffer == 0) return 0;
  if (ICM426_I2C_Read(&icmh->dev, (uint8_t) ICM_R_FIFO_COUNTH, &icmh->rxBuffer[0], 2) != ICM426_OK)
    return 0;

  fifoCount = (icmh->rxBuffer[0] << 8) & icmh->rxBuffer[1];
  ICM426_I2C_ReadByte(&icmh->dev, (uint8_t) ICM_R_MCLK_RDY, &icmh->rxBuffer[0]);

  while (fifoCount--) {
    ICM426_I2C_ReadByte(&icmh->dev, (uint8_t) ICM_R_FIFO_DATA, &icmh->rxBuffer[0]);
    readLen++;

    if (fifoPtr->writeOffset == 0) { // reading header
      if (!(icmh->rxBuffer[0] & ICM_P_HEADER_MSG) || (icmh->rxBuffer[0] == 0xFF))
        break;
      memset(raw, 0, sizeof(fifoPtr->rawPacket));
    }
    if (fifoPtr->writeOffset == 1) { // reading accel
      if (!(fifoPtr->rawPacket.header & ICM_P_HEADER_ACCEL))
        fifoPtr->writeOffset = 7;
    }
    if (fifoPtr->writeOffset == 7) { // reading gyro
      if (!(fifoPtr->rawPacket.header & ICM_P_HEADER_GYRO))
        fifoPtr->writeOffset = 13;
    }
    if (fifoPtr->writeOffset == 13) { // reading temperature extend
      if (!(fifoPtr->rawPacket.header & ICM_P_HEADER_20))
        fifoPtr->writeOffset = 14;
    }
    if (fifoPtr->writeOffset == 15) { // reading timestamp
      if (!(fifoPtr->rawPacket.header & ICM_P_HEADER_TIMESTAMP_FSYNC))
        fifoPtr->writeOffset = 17;
    }
    if (fifoPtr->writeOffset == 17) { // reading extend bits
      if (!(fifoPtr->rawPacket.header & ICM_P_HEADER_20))
        goto processPacket;
    }

    *(raw + fifoPtr->writeOffset) = icmh->rxBuffer[0];

    fifoPtr->writeOffset++;
    if (fifoPtr->writeOffset == sizeof(fifoPtr->rawPacket)) {
      processPacket:
      fifoPtr->writeOffset = 0;

      // write to buffer circulary
      dataPtr = fifoPtr->buffer + fifoPtr->wPosData;
      READ_RAW_DATA_BIGENDIAN(&dataPtr->accel.x, &fifoPtr->rawPacket.accel.x[0]);
      READ_RAW_DATA_BIGENDIAN(&dataPtr->accel.y, &fifoPtr->rawPacket.accel.y[0]);
      READ_RAW_DATA_BIGENDIAN(&dataPtr->accel.z, &fifoPtr->rawPacket.accel.z[0]);
      READ_RAW_DATA_BIGENDIAN(&dataPtr->gyro.x, &fifoPtr->rawPacket.gyro.x[0]);
      READ_RAW_DATA_BIGENDIAN(&dataPtr->gyro.y, &fifoPtr->rawPacket.gyro.y[0]);
      READ_RAW_DATA_BIGENDIAN(&dataPtr->gyro.z, &fifoPtr->rawPacket.gyro.z[0]);
      READ_RAW_DATA_BIGENDIAN(&dataPtr->temperature, &fifoPtr->rawPacket.temperature[0]);
      READ_RAW_DATA_BIGENDIAN(&dataPtr->timestamp, &fifoPtr->rawPacket.timestamp[0]);

      fifoPtr->wPosData++;
      if (fifoPtr->wPosData == fifoPtr->bufferSize)   fifoPtr->wPosData = 0;
      if (fifoPtr->wPosData == fifoPtr->rPosData)     fifoPtr->isWriteOverlap = 1;
      if (fifoPtr->isWriteOverlap
          && fifoPtr->rPosData != fifoPtr->wPosData)
      {
        fifoPtr->lostData++;
        fifoPtr->rPosData = fifoPtr->wPosData;
      }
    }
  }

  return readLen;
}


ICM426_Status_t ICM426_FIFO_Flush(ICM426_FIFO_t *fifoPtr)
{
  ICM426_Status_t status;
  ICM426_HandlerTypeDef *icmh = (ICM426_HandlerTypeDef*) fifoPtr->icmh;

  if ((status = ICM426_I2C_WriteByte(&icmh->dev, ICM_R_SIGNAL_PATH_RESET, 0x04)) != ICM426_OK) {
    return status;
  }
  fifoPtr->rPosData = 0;
  fifoPtr->wPosData = 0;
  fifoPtr->isWriteOverlap = 0;
  fifoPtr->writeOffset = 0;
  fifoPtr->lostData = 0;
  ICM426_Delay(2);
  return ICM426_OK;
}
