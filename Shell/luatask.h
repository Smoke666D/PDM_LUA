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
	LUA_INIT,
	LUA_RUN,
	LUA_ERROR,
	LUA_STOP,
	LUA_RESTART,
	LUA_STATE_SIZE
} LUA_STATE;


LUA_STATE xLUAgetSTATE();
const char * xLUAgetError();
int xLUAgetTime();
void vLUArunPDM();
void vLUAstopPDM();
void vLUArestartPDM();
EventGroupHandle_t* osLUAetPDMstatusHandle ( void );
void vLUArunPDM();
void vLUAstopPDM();
void vLuaTask(void *argument);

#endif /* LUATASK_H_ */
