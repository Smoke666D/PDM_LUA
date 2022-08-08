/*
 * luatask.h
 *
 *  Created on: Apr 6, 2022
 *      Author: igor.dymov
 */

#ifndef LUATASK_H_
#define LUATASK_H_

#include "main.h"
#include "cmsis_os.h"
#include "FreeRTOS.h"
#include "task.h"
osThreadId_t* osLUAgetTaskHandle ( void );
void vLuaTask(void *argument);

#endif /* LUATASK_H_ */
