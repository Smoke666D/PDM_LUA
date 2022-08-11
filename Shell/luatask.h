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


#define CONFIG_ARGUMENT_COUNT 4
#define RESET_ARGUMENT_COUNT 3
#define PWM_ARGUMENT_COUNT 2
#define CANSEND_ARGUMENT_COUNT 2
#define CANREQSEND_ARGUMENT_COUNT 3
#define SEND_REQUEST_ARGUMENT_COUNT 3


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
} LUA_STATE_t;


LUA_STATE_t eLUAgetSTATE();
const char * pcLUAgetErrorString( void );
uint32_t ulLUAgetWorkCicle( void );
void vLUArunPDM( void );
void vLUAstopPDM( void );
void vLUArestartPDM( void );
EventGroupHandle_t* osLUAetPDMstatusHandle ( void );
void vLUArunPDM( void );
void vLUAstopPDM( void );
void vLuaTask(void *argument);

#endif /* LUATASK_H_ */
