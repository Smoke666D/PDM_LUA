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

#include "serial.h"
/*-------------------------------- Structures --------------------------------*/
/*--------------------------------- Constant ---------------------------------*/
/*-------------------------------- Variables ---------------------------------*/
static TASK_ANALIZE tasks[SYS_MAX_TSAK_NUMBER] = { 0U };
static uint8_t      taskNumber                 = 0U;

/*
static uint32_t defaultTaskBuffer[DEFAULT_TASK_STACK_SIZE]      __section( TASK_RAM_SECTION ) = { 0U };
static uint32_t luaTaskBuffer[LUA_TASK_STACK_SIZE]              __section( TASK_RAM_SECTION ) = { 0U };
static uint32_t adcTaskBuffer[ADC_TASK_STACK_SIZE]              __section( TASK_RAM_SECTION ) = { 0U };
static uint32_t doutTaskBuffer[DOUT_TASK_STACK_SIZE]            __section( TASK_RAM_SECTION ) = { 0U };
static uint32_t canTaskBuffer[CAN_TASK_STACK_SIZE]              __section( TASK_RAM_SECTION ) = { 0U };
*/
static uint32_t serialTxTaskBuffer[SERIAL_TX_TSAK_STACK_SIZE]           __section( TASK_RAM_SECTION ) = { 0U };
static uint32_t serialRxTaskBuffer[SERIAL_RX_TSAK_STACK_SIZE]           __section( TASK_RAM_SECTION ) = { 0U };
static uint32_t serialProtectTaskBuffer[SERIAL_PROTECT_TSAK_STACK_SIZE] __section( TASK_RAM_SECTION ) = { 0U };

/*
static StaticTask_t defaultTaskControlBlock       __section( TASK_RAM_SECTION ) = { 0U };
static StaticTask_t luaTaskControlBlock           __section( TASK_RAM_SECTION ) = { 0U };
static StaticTask_t adcTaskControlBlock           __section( TASK_RAM_SECTION ) = { 0U };
static StaticTask_t doutTaskControlBlock          __section( TASK_RAM_SECTION ) = { 0U };
static StaticTask_t canTaskControlBlock           __section( TASK_RAM_SECTION ) = { 0U };
*/
static StaticTask_t serialTxTaskControlBlock      __section( TASK_RAM_SECTION ) = { 0U };
static StaticTask_t serialRxTaskControlBlock      __section( TASK_RAM_SECTION ) = { 0U };
static StaticTask_t serialProtectTaskControlBlock __section( TASK_RAM_SECTION ) = { 0U };

/*
static osThreadId_t defaultTaskHandle       = NULL;
static osThreadId_t luaTaskHandle           = NULL;
static osThreadId_t adcTaskHandle           = NULL;
static osThreadId_t doutTaskHandle          = NULL;
static osThreadId_t canTaskHandle           = NULL;
*/

/*
static uint8_t canTXBuffer[ 16U * sizeof( CO_CANtx_t ) ]     __section( TASK_RAM_SECTION ) = { 0U };
static uint8_t canRXBuffer[ 16U * sizeof( CAN_FRAME_TYPE ) ] __section( TASK_RAM_SECTION ) = { 0U };

static osMessageQueueId_t canTXHandle  = NULL;
static osMessageQueueId_t canRXHandle  = NULL;
*/

static uint8_t serialOutputBuffer[ SERIAL_QUEUE_SIZE * sizeof( UART_MESSAGE ) ] __section( TASK_RAM_SECTION ) = { 0U };

static StaticQueue_t xSERIALqueue __section( TASK_RAM_SECTION ) = { 0U };
/*-------------------------------- Functions ---------------------------------*/

/*----------------------------------------------------------------------------*/
/*----------------------- PRIVATE --------------------------------------------*/
/*----------------------------------------------------------------------------*/
static void vSYSprintLoadBar ( uint8_t procents )
{
  uint8_t i       = 0U;
  uint8_t wm      = ( uint8_t )( procents / SYS_BAR_LENGTH );
  char    buf[4U] = { " " };
  if ( procents > 100U )
  {
    wm = 100U / SYS_BAR_LENGTH;
  }
  for ( i=0U; i<wm; i++ )
  {
    vSYSserial( "█", 1U );
  }
  for ( i=wm; i<SYS_BAR_LENGTH; i++ )
  {
    vSYSserial( "▒", 1U );
  }
  ( void )sprintf( buf, " %u", procents );
  vSYSserial( buf, strlen( buf ) );
  vSYSserial( "% ", 2U );
  return;
}
/*----------------------------------------------------------------------------*/
static void vSYSprintUsedData ( uint16_t usage, uint16_t total )
{
  char buf[10U] = { " " };
  ( void )sprintf( buf, " %u", usage );
  vSYSserial( buf, strlen( buf ) );
  vSYSserial( "/", 1U );
  ( void )sprintf( buf, " %u", total );
  vSYSserial( " bytes\r\n", 8U );
  return;
}
/*----------------------------------------------------------------------------*/
static void vSYSprintHeapData ( void )
{
  uint16_t usage = xPortGetMinimumEverFreeHeapSize();
  uint8_t  used  = ( uint8_t )( usage * 100U / configTOTAL_HEAP_SIZE );
  vSYSserial( "Heap             : ", 19U );
  vSYSprintLoadBar( used );
  vSYSprintUsedData( usage, ( uint16_t )( configTOTAL_HEAP_SIZE ) );
  return;
}
/*----------------------------------------------------------------------------*/
static void vSYSprintTaskData ( TASK_ANALIZE task )
{
  uint16_t usage = ( uint16_t )( task.size - uxTaskGetStackHighWaterMark( task.thread ) * 4U );
  uint8_t  used  = ( uint8_t )( task.size / usage );
  char*    name  = pcTaskGetName( task.thread );
  vSYSserial( name, strlen( name ) );
  vSYSserial( " : ", 3U );
  vSYSprintLoadBar( used );
  vSYSprintUsedData( usage, ( uint16_t )( task.size ) );
  return;
}
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
    .stack_size = sizeof( *stackAlloc ),
    .priority   = priority
  };
  *thread = osThreadNew( func, NULL, &task_attributes );
  //vSYSaddTask( thread, task_attributes.stack_size );
  return;
}
/*----------------------------------------------------------------------------*/
/*----------------------- PABLICK --------------------------------------------*/
/*----------------------------------------------------------------------------*/
void vSYStaskInit ( void )
{
  vSYSstaticTaskInit( serialTxTaskBuffer, &serialTxTaskControlBlock, SERIAL_TX_TASK_NAME, osSERIALgetSerialTxTaskHandle(), SERIAL_TX_TASK_PRIORITY, vSERIALtxTask );
  vSYSstaticTaskInit( serialRxTaskBuffer, &serialRxTaskControlBlock, SERIAL_RX_TASK_NAME, osSERIALgetSerialRxTaskHandle(), SERIAL_RX_TASK_PRIORITY,vSERIALrxTask );
  vSYSstaticTaskInit( serialProtectTaskBuffer, &serialProtectTaskControlBlock, SERIAL_PROTECT_TASK_NAME, osSERIALgetSerialProtectTaskHandle(), SERIAL_PROTECT_TASK_PRIORITY, vSERIALprotectTask );
  return;
}
/*----------------------------------------------------------------------------*/
void vSYSqueueInit ( void )
{
  *( pSERIALgetQueue() ) = xQueueCreateStatic( SERIAL_QUEUE_SIZE, sizeof( UART_MESSAGE ), ( uint8_t* )serialOutputBuffer, &xSERIALqueue );
}
/*----------------------------------------------------------------------------*/
void vSYSprintData ( void )
{
  vSYSserial( "------------------ freeRTOS ------------------\n", 47U );
  vSYSprintHeapData();
  for ( uint8_t i=0U; i<taskNumber; i++ )
  {
    vSYSprintTaskData( tasks[i] );
    vSYSserial( "\n", 1U );
  }
  vSYSserial( "----------------------------------------------\n", 47U );
  return;
}
/*----------------------------------------------------------------------------*/
void vSYSgetHeapData ( SYSTEM_DATA* data )
{
  data->name  = "heap";
  data->usage = xPortGetMinimumEverFreeHeapSize();
  data->total = configTOTAL_HEAP_SIZE;
  return;
}
/*----------------------------------------------------------------------------*/
uint8_t uSYSgetTaskumber ( void )
{
  return taskNumber;
}
/*----------------------------------------------------------------------------*/
void vSYSgetTaskData ( uint8_t n, SYSTEM_DATA* data )
{
  if ( n < SYS_MAX_TSAK_NUMBER )
  {
    data->name  = pcTaskGetName( tasks[n].thread );
    data->total = tasks[n].size;
    data->usage = ( uint16_t )( tasks[n].size - uxTaskGetStackHighWaterMark( tasks[n].thread ) * 4U );
  }
  return;
}
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
