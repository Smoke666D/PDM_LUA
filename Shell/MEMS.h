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
#define ACC_ODR  ((float)ALGO_FREQ)
#define ACC_FS  2 /* FS = <-2g, 2g> */

void vmemsTask(void *argument);
void MX_DynamicInclinometer_Process();
float fAngleGet ( ANGLE_TYPE type );
#endif /* MEMS_H_ */
