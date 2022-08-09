/*
 * luatask.h
 *
 *  Created on: Apr 6, 2022
 *      Author: igor.dymov
 */

#ifndef LUATASK_H_
#define LUATASK_H_

#include "main.h"
#include "FreeRTOS.h"
#include "cmsis_os.h"
#include "event_groups.h"


#define RUN_STATE  0x01

EventGroupHandle_t* osLUAetPDMstatusHandle ( void );
void vLUArunPDM();
void vLUAstopPDM();
void vLuaTask(void *argument);

#endif /* LUATASK_H_ */
