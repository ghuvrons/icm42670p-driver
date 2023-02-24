/*
 * icm-fifo.c
 *
 *  Created on: Jul 27, 2022
 *      Author: janoko
 */


#include "include/icm-426.h"
#include "include/icm-426/utils.h"
#include <string.h>


ICM426_Status_t ICM426_FIFO_Start(ICM426_FIFO_t *fifoPtr)
{
  ICM426_Status_t status;
  uint8_t isClockReady;
  ICM426_HandlerTypeDef *icmh     = (ICM426_HandlerTypeDef*) fifoPtr->icmh;

  if (icmh == 0) return ICM426_ERROR;

  uint16_t fifoWatermark = 100 & 0x0FFF;
  uint8_t myB = 0;

  ICM426_SwitchMode(icmh, ICM_PWR_ACCEL_OFF|ICM_PWR_GYRO_OFF|ICM_PWR_RC_OCILATOR_ON);

  status = ICM426_REG_WriteByte(icmh, ICM_R_FDR_CONFIG, 0b1001);
  if (status != ICM426_OK) return status;

  ICM426_SwitchMode(icmh, ICM_PWR_ACCEL_LN_MODE|ICM_PWR_GYRO_LN|ICM_PWR_RC_OCILATOR_ON);

checkClock:
  status = ICM426_REG_ReadByte(icmh, ICM_R_MCLK_RDY, &isClockReady);
  if (status != ICM426_OK) return status;

  if ((isClockReady&0x8) == 0) {
    icmh->delay(2);
    goto checkClock;
  }

  // disable FIFO
  status = ICM426_REG_WriteByte(icmh, ICM_R_FIFO_CONFIG1, 0x01);
  if (status != ICM426_OK) return status;

  status = ICM426_REG_WriteByte(icmh, ICM_R_FIFO_CONFIG2, fifoWatermark & 0x00FF);
  if (status != ICM426_OK) return status;

  status = ICM426_REG_WriteByte(icmh, ICM_R_FIFO_CONFIG3, fifoWatermark >> 8);
  if (status != ICM426_OK) return status;

  myB = (fifoPtr->config.feature_en & ICM_FIFO_CONFIG5_MASK) >> 2;
  status = ICM426_REG_WriteByte(icmh, ICM_R_FIFO_CONFIG5, myB);
  if (status != ICM426_OK) return status;

  status = ICM426_REG_WriteByte(icmh, ICM_R_TMST_CONFIG1, 0x00);
  if (status != ICM426_OK) return status;

  status = ICM426_FIFO_Flush(fifoPtr);
  if (status != ICM426_OK) return status;

  // enable FIFO
  status = ICM426_REG_WriteByte(icmh, ICM_R_FIFO_CONFIG1, 0x00);
  if (status != ICM426_OK) return status;

  return ICM426_OK;
}


uint16_t ICM426_FIFO_GetFIFOCount(ICM426_FIFO_t *fifoPtr)
{
  uint16_t fifoCount = 0;

  if (((ICM426_HandlerTypeDef*) fifoPtr->icmh)->i2c.read((uint8_t) ICM_R_FIFO_COUNTH,
                                                         (uint8_t*) &fifoCount,
                                                         sizeof(uint16_t)) != ICM426_OK)
  {
    return 0;
  }

  return fifoCount;
}


uint16_t ICM426_FIFO_GetData(ICM426_FIFO_t *fifoPtr, ICM426_Data_t *bufferData, uint16_t size)
{
  uint16_t              readLen   = 0;
  uint16_t              fifoCount = 0;
  ICM426_HandlerTypeDef *icmh     = (ICM426_HandlerTypeDef*) fifoPtr->icmh;
  uint8_t               *packet;
  uint8_t               tmpPacket[20];

  if (bufferData == 0) return 0;

  uint8_t packetSize = 2      // header + temperature data
          + ((fifoPtr->config.feature_en & ICM_FIFO_ACCEL_EN)? 6:0)
          + ((fifoPtr->config.feature_en & ICM_FIFO_GYRO_EN)? 6:0)
          + (((fifoPtr->config.feature_en & ICM_FIFO_ACCEL_EN) &&
              (fifoPtr->config.feature_en & ICM_FIFO_GYRO_EN))? 2:0) // 2 bytes timestamps
          + ((fifoPtr->config.feature_en & ICM_FIFO_HIRES_EN)? 4:0); // extension bits

  if (icmh->i2c.read((uint8_t) ICM_R_FIFO_COUNTH,
                     (uint8_t*) &fifoCount, sizeof(uint16_t)) != ICM426_OK)
  {
    return 0;
  }

  while (fifoCount--) {
    if (icmh->i2c.read((uint8_t) ICM_R_FIFO_DATA,
                       (uint8_t*) &tmpPacket[0],
                       packetSize) != ICM426_OK)
    {
      break;
    }

    packet = &tmpPacket[0];

    // check header, continue if empty
    if ((tmpPacket[0] & ICM_P_HEADER_MSG) != 0 || size == 0) {
      continue;
    }

    readLen++;
    memset(bufferData, 0, sizeof(ICM426_Data_t));

    packet += 1;  // skip header

    if ((tmpPacket[0] & ICM_P_HEADER_ACCEL) != 0) {
      bufferData->accel.x = ((float) *((int16_t *) packet))
                            *icmh->scaleFactor.accel;
      packet += 2; // next data

      bufferData->accel.y = ((float) *((int16_t *) packet))
                            *icmh->scaleFactor.accel;
      packet += 2; // next data

      bufferData->accel.z = ((float) *((int16_t *) packet))
                            *icmh->scaleFactor.accel;
      packet += 2; // next data
    }

    if ((tmpPacket[0] & ICM_P_HEADER_GYRO) != 0) {
      bufferData->gyro.x = ((float) *((int16_t *) packet))
                           *icmh->scaleFactor.gyro;
      packet += 2; // next data

      bufferData->gyro.y = ((float) *((int16_t *) packet))
                           *icmh->scaleFactor.gyro;
      packet += 2; // next data

      bufferData->gyro.z = ((float) *((int16_t *) packet))
                           *icmh->scaleFactor.gyro;
      packet += 2; // next data
    }

    bufferData++;
    size--;
  }

  return readLen;
}


ICM426_Status_t ICM426_FIFO_Flush(ICM426_FIFO_t *fifoPtr)
{
  ICM426_Status_t status;
  ICM426_HandlerTypeDef *icmh = (ICM426_HandlerTypeDef*) fifoPtr->icmh;
  uint8_t signalPathReset;

  if ((status = ICM426_REG_ReadByte(icmh,
                                    ICM_R_SIGNAL_PATH_RESET,
                                    &signalPathReset)) != ICM426_OK)
  {
    return status;
  }

  if ((status = ICM426_REG_WriteByte(icmh,
                                     ICM_R_SIGNAL_PATH_RESET,
                                     (signalPathReset&0x03))) != ICM426_OK)
  {
    return status;
  }

  icmh->delay(2);
  return ICM426_OK;
}
