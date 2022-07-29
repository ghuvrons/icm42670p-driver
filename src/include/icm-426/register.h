/*
 * register.h
 *
 *  Created on: Jul 28, 2022
 *      Author: janoko
 */

#ifndef DRIVERS_ICM_426_REGISTER_H_
#define DRIVERS_ICM_426_REGISTER_H_

#define ICM_R_MCLK_RDY          0x0000U
#define ICM_R_DEVICE_CONFIG     0x0001U
#define ICM_R_SIGNAL_PATH_RESET 0x0002U
#define ICM_R_DRIVE_CONFIG1     0x0003U
#define ICM_R_DRIVE_CONFIG2     0x0004U
#define ICM_R_DRIVE_CONFIG3     0x0005U
#define ICM_R_INT_CONFIG        0x0006U
#define ICM_R_TEMP_DATA1        0x0009U
#define ICM_R_TEMP_DATA0        0x000AU
#define ICM_R_ACCEL_DATA_X1     0x000BU
#define ICM_R_ACCEL_DATA_X0     0x000CU
#define ICM_R_ACCEL_DATA_Y1     0x000DU
#define ICM_R_ACCEL_DATA_Y0     0x000EU
#define ICM_R_ACCEL_DATA_Z1     0x000FU
#define ICM_R_ACCEL_DATA_Z0     0x0010U
#define ICM_R_GYRO_DATA_X1      0x0011U
#define ICM_R_GYRO_DATA_X0      0x0012U
#define ICM_R_GYRO_DATA_Y1      0x0013U
#define ICM_R_GYRO_DATA_Y0      0x0014U
#define ICM_R_GYRO_DATA_Z1      0x0015U
#define ICM_R_GYRO_DATA_Z0      0x0016U
#define ICM_R_TMST_FSYNCH       0x0017U
#define ICM_R_TMST_FSYNCL       0x0018U
#define ICM_R_APEX_DATA4        0x001DU
#define ICM_R_APEX_DATA5        0x001EU
#define ICM_R_PWR_MGMT0         0x001FU
#define ICM_R_GYRO_CONFIG0      0x0020U
#define ICM_R_ACCEL_CONFIG0     0x0021U
#define ICM_R_TEMP_CONFIG0      0x0022U
#define ICM_R_GYRO_CONFIG1      0x0023U
#define ICM_R_ACCEL_CONFIG1     0x0024U
#define ICM_R_APEX_CONFIG0      0x0025U
#define ICM_R_APEX_CONFIG1      0x0026U
#define ICM_R_WOM_CONFIG        0x0027U
#define ICM_R_FIFO_CONFIG1      0x0028U
#define ICM_R_FIFO_CONFIG2      0x0029U
#define ICM_R_FIFO_CONFIG3      0x002AU
#define ICM_R_INT_SOURCE0       0x002BU
#define ICM_R_INT_SOURCE1       0x002CU
#define ICM_R_INT_SOURCE3       0x002DU
#define ICM_R_INT_SOURCE4       0x002EU
#define ICM_R_FIFO_LOST_PKT0    0x002FU
#define ICM_R_FIFO_LOST_PKT1    0x0030U
#define ICM_R_APEX_DATA0        0x0031U
#define ICM_R_APEX_DATA1        0x0032U
#define ICM_R_APEX_DATA2        0x0033U
#define ICM_R_APEX_DATA3        0x0034U
#define ICM_R_INTF_CONFIG0      0x0035U
#define ICM_R_INTF_CONFIG1      0x0036U
#define ICM_R_INT_STATUS_DRDY   0x0039U
#define ICM_R_INT_STATUS        0x003AU
#define ICM_R_INT_STATUS2       0x003BU
#define ICM_R_INT_STATUS3       0x003CU
#define ICM_R_FIFO_COUNTH       0x003DU
#define ICM_R_FIFO_COUNTL       0x003EU
#define ICM_R_FIFO_DATA         0x003FU
#define ICM_R_WHO_AM_I          0x0075U
#define ICM_R_BLK_SEL_W         0x0079U
#define ICM_R_MADDR_W           0x007AU
#define ICM_R_M_W               0x007BU
#define ICM_R_BLK_SEL_R         0x007CU
#define ICM_R_MADDR_R           0x007DU
#define ICM_R_M_R               0x007EU

#define ICM_R_TMST_CONFIG1      0x0100U
#define ICM_R_FIFO_CONFIG5      0x0101U
#define ICM_R_FIFO_CONFIG6      0x0102U
#define ICM_R_FSYNC_CONFIG      0x0103U
#define ICM_R_INT_CONFIG0       0x0104U
#define ICM_R_INT_CONFIG1       0x0105U
#define ICM_R_SENSOR_CONFIG3    0x0106U
#define ICM_R_ST_CONFIG         0x0113U
#define ICM_R_SELFTEST          0x0114U
#define ICM_R_INTF_CONFIG6      0x0123U
#define ICM_R_INTF_CONFIG10     0x0125U
#define ICM_R_INTF_CONFIG7      0x0128U
#define ICM_R_OTP_CONFIG        0x012BU
#define ICM_R_INT_SOURCE6       0x012FU
#define ICM_R_INT_SOURCE7       0x0130U
#define ICM_R_INT_SOURCE8       0x0131U
#define ICM_R_INT_SOURCE9       0x0132U
#define ICM_R_INT_SOURCE10      0x0133U
#define ICM_R_APEX_CONFIG2      0x0144U
#define ICM_R_APEX_CONFIG3      0x0145U
#define ICM_R_APEX_CONFIG4      0x0146U
#define ICM_R_APEX_CONFIG5      0x0147U
#define ICM_R_APEX_CONFIG9      0x0148U
#define ICM_R_APEX_CONFIG10     0x0149U
#define ICM_R_APEX_CONFIG11     0x014AU
#define ICM_R_ACCEL_WOM_X_THR   0x014BU
#define ICM_R_ACCEL_WOM_Y_THR   0x014CU
#define ICM_R_ACCEL_WOM_Z_THR   0x014DU
#define ICM_R_OFFSET_USER0      0x014EU
#define ICM_R_OFFSET_USER1      0x014FU
#define ICM_R_OFFSET_USER2      0x0150U
#define ICM_R_OFFSET_USER3      0x0151U
#define ICM_R_OFFSET_USER4      0x0152U
#define ICM_R_OFFSET_USER5      0x0153U
#define ICM_R_OFFSET_USER6      0x0154U
#define ICM_R_OFFSET_USER7      0x0155U
#define ICM_R_OFFSET_USER8      0x0156U
#define ICM_R_ST_STATUS1        0x0163U
#define ICM_R_ST_STATUS2        0x0164U
#define ICM_R_FDR_CONFIG        0x0166U
#define ICM_R_APEX_CONFIG12     0x0167U

#define ICM_R_OTP_CTRL7         0x2906U

#define ICM_R_XA_ST_DATA        0x5100U
#define ICM_R_YA_ST_DATA        0x5101U
#define ICM_R_ZA_ST_DATA        0x5102U
#define ICM_R_XG_ST_DATA        0x5103U
#define ICM_R_YG_ST_DATA        0x5104U
#define ICM_R_ZG_ST_DATA        0x5105U


#endif /* DRIVERS_ICM_426_REGISTER_H_ */
