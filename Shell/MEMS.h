/*
 * MEMS.h
 *
 *  Created on: Nov 11, 2022
 *      Author: igor.dymov
 */

#ifndef MEMS_H_
#define MEMS_H_

#include "main.h"
#include "FreeRTOS.h"
#include "cmsis_os.h"
#include "event_groups.h"
#include "luatask.h"
#include "system.h"
#include "task.h"
#include "semphr.h"
#include "data.h"
#include "stm32f4xx_hal.h"


#define ALGO_FREQ  100U
#define DECIMATION  4U
#define TMsg_MaxLen             256
typedef struct
{
  uint32_t Len;
  uint8_t Data[TMsg_MaxLen];
} TMsg;
#define STREAMING_MSG_LENGTH  71

/* Exported types ------------------------------------------------------------*/
typedef struct {
  uint8_t hours;
  uint8_t minutes;
  uint8_t seconds;
  uint8_t subsec;
  float pressure;
  float humidity;
  float temperature;
  int32_t acceleration_x_mg;
  int32_t acceleration_y_mg;
  int32_t acceleration_z_mg;
  int32_t angular_rate_x_mdps;
  int32_t angular_rate_y_mdps;
  int32_t angular_rate_z_mdps;
  int32_t magnetic_field_x_mgauss;
  int32_t magnetic_field_y_mgauss;
  int32_t magnetic_field_z_mgauss;
} offline_data_t;

#define SENDER_UART  0x01
#define SENDER_USB   0x02
#define SENDER_SPI   0x03

#define DEV_ADDR  50U
#define I2C_DATA_MAX_LENGTH_BYTES  16
#define MIN(A,B) ((A)<(B)?(A):(B))
#define OFFLINE_DATA_SIZE  8

/* Enable sensor masks */
#define PRESSURE_SENSOR       0x00000001U
#define TEMPERATURE_SENSOR    0x00000002U
#define HUMIDITY_SENSOR       0x00000004U
#define UV_SENSOR             0x00000008U /* for future use */
#define ACCELEROMETER_SENSOR  0x00000010U
#define GYROSCOPE_SENSOR      0x00000020U
#define MAGNETIC_SENSOR       0x00000040U

#define STREAMING_MSG_LENGTH  71

/* Algorithm-based defines */
#define CMD_ANGLE_MODE_PITCH_ROLL_GRAVITY_INCLINATION  0x01
#define CMD_ANGLE_MODE_THETA_PSI_PHI                   0x02
#define CMD_CAL_POS_LYING_NORMAL_ON_TABLE              0x03
#define CMD_CAL_POS_LYING_UPSIDEDOWN_ON_TABLE          0x04
#define CMD_CAL_POS_NUCLEO_CONNECTOR_DOWN              0x05
#define CMD_CAL_POS_NUCLEO_CONNECTOR_LEFT              0x06
#define CMD_CAL_POS_NUCLEO_CONNECTOR_UP                0x07
#define CMD_CAL_POS_NUCLEO_CONNECTOR_RIGHT             0x08
#define CMD_GET_CALIBRATION_COEFFICIENTS               0x09
#define CMD_GET_ESTIMATED_MEASUREMENT_TIME             0x0A

#define REQUIRED_DATA  (ACCELEROMETER_SENSOR)
float fAngleGet ( ANGLE_TYPE type );
void vmemsTask(void *argument);
void MX_DynamicInclinometer_Process();




#endif /* MEMS_H_ */
