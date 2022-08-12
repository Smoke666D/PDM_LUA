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

#define MAX_SCRIPT_SIZE LUA_HEAP_SIZE

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

typedef enum
{
	RESULT_TRUE = 1,
	RESULT_FALSE =0
} RESULT_t;

typedef enum
{
	RUN_DEFAULT_SCRIPT = 1U,
	RUN_USER_SCRIPT = 0U,
} RUN_SCRIPT_t;


LUA_STATE_t eLUAgetSTATE();
const char* pcLUAgetErrorString( void );
uint8_t     ucLUAgetErrorCount ( void );
uint32_t    ulLUAgetWorkCicle( void );
void        vLUArunPDM( void );
void        vLUAstopPDM( void );
void        vLUArestartPDM( void );
void        vLUArunPDM( void );
void        vLUAstopPDM( void );
void        vLuaTask(void *argument);
EventGroupHandle_t* osLUAetPDMstatusHandle ( void );

#endif /* LUATASK_H_ */
