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
#define STOPR_STATE  0x02
#define RESTART_STATE  0x04

typedef enum {
	LUA_INIT = 0,
	LUA_RUN = 1,
	LUA_ERROR = 2,
	LUA_STOP =3,
	LUA_RESTART = 4,
} LUA_STATE;


void vLUArunPDM();
void vLUAstopPDM();
void vLUArestartPDM();
EventGroupHandle_t* osLUAetPDMstatusHandle ( void );
void vLUArunPDM();
void vLUAstopPDM();
void vLuaTask(void *argument);

#endif /* LUATASK_H_ */
