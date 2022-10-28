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

#define MAX_SCRIPT_SIZE LUA_HEAP_SIZE


#define CAN_FRAME_SIZE  				8U
#define CAN_ID_POS					    1U
#define FRAME_SIZE_POS					2U
#define CAN_TABLE_POS					3U

#define CONFIG_ARGUMENT_COUNT 4
#define RESET_ARGUMENT_COUNT 3
#define PWM_ARGUMENT_COUNT 2
#define CANSEND_ARGUMENT_COUNT 2
#define CANREQSEND_ARGUMENT_COUNT 3
#define SEND_REQUEST_ARGUMENT_COUNT 3
#define CAN_SEND_TABLE_ARGUMENT_COUNT 	3U


#define FIRST_ARGUMENT   1U
#define SECOND_ARGUMENT  2U
#define LAST_ARGUMENT	-1

#define ONE_ARGUMENT	1U
#define TWO_ARGUMENT	2U

#define END_OF_FILE_BYTE  0xFF


#define RUN_STATE  0x01
#define STOPR_STATE  0x02
#define RESTART_STATE  0x04


#define RPM_STATE   0x02

#define NO_RESULT  0U
#define ONE_RESULT 1U
#define TWO_RESULT 2U
#define SUCSESS    1U
#define ERROR	   0U


typedef enum {
	LUA_INIT,
	LUA_RUN,
	LUA_ERROR,
	LUA_STOP,
	LUA_RESTART,
	LUA_STATE_SIZE
} LUA_STATE_t;

typedef enum {
	IS_ENABLE  =0,
	IS_DISABLE  =1
} ENABLE_t;

typedef enum
{
	RESULT_TRUE = 1,
	RESULT_FALSE =0
} RESULT_t;



typedef enum
{
  ERROR_NO  = 0,
  BUFFER_FULL = 1
} ERROR_TYPE_t;





char * pcGetLUAError();
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
