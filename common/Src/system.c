/*
 * system.c
 *
 *  Created on: 8 нояб. 2021 г.
 *      Author: 79110
 */
/*--------------------------------- Includes ---------------------------------*/
#include "system.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include <stdio.h>
#include <string.h>
#include "pdm_input.h"

#include "usbhid.h"
#include "luatask.h"
#include "cantask.h"
#include "PDMhardware.h"
#include "mems.h"

/*-------------------------------- Structures --------------------------------*/
/*--------------------------------- Constant ---------------------------------*/
/*-------------------------------- Variables ---------------------------------*/
static TASK_ANALIZE tasks[SYS_MAX_TSAK_NUMBER] = { 0U };
static uint8_t      taskNumber                 = 0U;


static uint32_t memsTaskBuffer[MEMS_TASK_STACK_SIZE]            		  __section( TASK_RAM_SECTION ) = { 0U };
static uint32_t luaTaskBuffer[LUA_TASK_STACK_SIZE]            			  __section( TASK_RAM_SECTION ) = { 0U };
static uint32_t canTXTaskBuffer[CAN_TX_TASK_STACK_SIZE]             		    __section( TASK_RAM_SECTION ) = { 0U };
static uint32_t canRXTaskBuffer[CAN_RX_TASK_STACK_SIZE]             		    __section( TASK_RAM_SECTION ) = { 0U };
static uint32_t usbTaskBuffer[USB_TASK_STACK_SIZE]                      __section( TASK_RAM_SECTION ) = { 0U };
static uint32_t dinTaskBuffer[DIN_TASK_STACK_SIZE]                      __section( TASK_RAM_SECTION ) = { 0U };
static uint32_t adcTaskBuffer[ADC_TASK_STACK_SIZE]              		__section( TASK_RAM_SECTION ) = { 0U };



static StaticTask_t luaTaskControlBlock           __section( TASK_RAM_SECTION ) = { 0U };
static StaticTask_t canRXTaskControlBlock           __section( TASK_RAM_SECTION ) = { 0U };
static StaticTask_t canTXTaskControlBlock           __section( TASK_RAM_SECTION ) = { 0U };
static StaticTask_t usbTaskControlBlock           __section( TASK_RAM_SECTION ) = { 0U };
static StaticTask_t dinTaskControlBlock           __section( TASK_RAM_SECTION ) = { 0U };
static StaticTask_t adcTaskControlBlock           __section( TASK_RAM_SECTION ) = { 0U };
static StaticTask_t memsTaskControlBlock           __section( TASK_RAM_SECTION ) = { 0U };


static osThreadId_t luaTaskHandle           = NULL;
static osThreadId_t dinTaskHandle          	    __section( TASK_RAM_SECTION ) = NULL;
static osThreadId_t canRXTaskHandle               __section( TASK_RAM_SECTION ) = NULL;
static osThreadId_t canTXTaskHandle               __section( TASK_RAM_SECTION ) = NULL;
static osThreadId_t adcTaskHandle         	    __section( TASK_RAM_SECTION ) = NULL;
static osThreadId_t memsTaskHandle         	    __section( TASK_RAM_SECTION ) = NULL;




static StaticEventGroup_t xPDMstatusEventGroup  __section( TASK_RAM_SECTION ) = { 0 };
static StaticEventGroup_t xCANstatusEventGroup  __section( TASK_RAM_SECTION ) = { 0 };
static uint8_t canTXBuffer[ CANTX_QUEUE_SIZE * sizeof( CAN_TX_FRAME_TYPE ) ]    __section( TASK_RAM_SECTION ) = { 0U };
static uint8_t canRXBuffer[ CANRX_QUEUE_SIZE * sizeof( CAN_FRAME_TYPE )  ] 		__section( TASK_RAM_SECTION ) = { 0U };
//static uint8_t can2TXBuffer[ CANTX_QUEUE_SIZE * sizeof( CAN_TX_FRAME_TYPE ) ]   __section( TASK_RAM_SECTION ) = { 0U };
//static uint8_t can2RXBuffer[ CANRX_QUEUE_SIZE * sizeof( CAN_FRAME_TYPE )  ] 	__section( TASK_RAM_SECTION ) = { 0U };


static StaticQueue_t xcanRXqueue __section( TASK_RAM_SECTION ) = { 0U };
static StaticQueue_t xcanTXqueue __section( TASK_RAM_SECTION ) = { 0U };
//static StaticQueue_t xcan2RXqueue __section( TASK_RAM_SECTION ) = { 0U };
//static StaticQueue_t xcan2TXqueue __section( TASK_RAM_SECTION ) = { 0U };
/*-------------------------------- Functions ---------------------------------*/

/*----------------------------------------------------------------------------*/
/*----------------------- PRIVATE --------------------------------------------*/
/*----------------------------------------------------------------------------*/




/*----------------------------------------------------------------------------*/
static void vSYSaddTask ( osThreadId_t thread, uint32_t size )
{
  if ( taskNumber < SYS_MAX_TSAK_NUMBER )
  {
    tasks[taskNumber].thread = thread;
    tasks[taskNumber].size   = size;
    taskNumber++;
  }
  return;
}
/*----------------------------------------------------------------------------*/
static void vSYSstaticTaskInit ( uint32_t*      stackAlloc,
								 uint32_t		sizeofstack,
                                 StaticTask_t*  controlBlock,
                                 const char*    name,
                                 osThreadId_t*  thread, 
                                 osPriority_t   priority, 
                                 osThreadFunc_t func )
{
  const osThreadAttr_t task_attributes = {
    .name       = name,
    .cb_mem     = controlBlock,
    .cb_size    = sizeof( *controlBlock ),
    .stack_mem  = stackAlloc,
    .stack_size = sizeofstack,
    .priority   = priority
  };
  *thread = osThreadNew( func, NULL, &task_attributes );
  vSYSaddTask( thread, task_attributes.stack_size );
  return;
}
/*----------------------------------------------------------------------------*/
/*----------------------- PABLICK --------------------------------------------*/
/*----------------------------------------------------------------------------*/
void vSYStaskInit ( void )
{
  vSYSstaticTaskInit( luaTaskBuffer, sizeof(luaTaskBuffer),&luaTaskControlBlock, LUA_TASK_NAME, &luaTaskHandle, LUA_TASK_PRIORITY, vLuaTask);
  vSYSstaticTaskInit( usbTaskBuffer, sizeof(usbTaskBuffer),&usbTaskControlBlock, USB_TASK_NAME, osUSBgetTaskHandle(), USB_TASK_PRIORITY, vUSBtask );
  vSYSstaticTaskInit( dinTaskBuffer, sizeof(dinTaskBuffer),&dinTaskControlBlock, DIN_TASK_NAME, &dinTaskHandle, DIN_TASK_PRIORITY, vDinTask);
  vSYSstaticTaskInit( canTXTaskBuffer, sizeof(canTXTaskBuffer),&canTXTaskControlBlock, CAN_TX_TASK_NAME, &canTXTaskHandle, CAN_TX_TASK_PRIORITY, vCanTXTask);
  vSYSstaticTaskInit( canRXTaskBuffer, sizeof(canRXTaskBuffer),&canRXTaskControlBlock, CAN_RX_TASK_NAME, &canRXTaskHandle, CAN_RX_TASK_PRIORITY, vCanRXTask);
  vSYSstaticTaskInit( adcTaskBuffer, sizeof(adcTaskBuffer),&adcTaskControlBlock, ADC_TASK_NAME, &adcTaskHandle, ADC_TASK_PRIORITY, vADCTask);
  vSYSstaticTaskInit( memsTaskBuffer, sizeof(memsTaskBuffer),&memsTaskControlBlock, MEMS_TASK_NAME, &memsTaskHandle, MEMS_TASK_PRIORITY, vmemsTask);
  return;
}
/*----------------------------------------------------------------------------*/
void vSYSqueueInit ( void )
{

  *( pCANRXgetQueue() ) = xQueueCreateStatic( CANRX_QUEUE_SIZE, sizeof( CAN_FRAME_TYPE), ( uint8_t* )canRXBuffer, &xcanRXqueue );
  *( pCANTXgetQueue() ) = xQueueCreateStatic( CANTX_QUEUE_SIZE, sizeof( CAN_TX_FRAME_TYPE ), ( uint8_t* )canTXBuffer, &xcanTXqueue );
}
/*----------------------------------------------------------------------------*/
void vSYSeventInit ( void )
{
*(osLUAetPDMstatusHandle () ) = xEventGroupCreateStatic(&xPDMstatusEventGroup );
*(osCANstatusHandle () )      = xEventGroupCreateStatic(&xCANstatusEventGroup );
}



/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
