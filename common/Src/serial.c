/*
 * common.c
 *
 *  Created on: 19 мар. 2020 г.
 *      Author: photo_Mickey
 */
/*----------------------- Includes ------------------------------------------------------------------*/
#include "serial.h"
#include "stm32f4xx_hal.h"
#include "cli.h"
#include "common.h"
#include "string.h"
#include "stdio.h"
#include "system.h"
#include "task.h"
#include "semphr.h"
#include "event_groups.h"
#include "stdlib.h"
/*----------------------- Structures ----------------------------------------------------------------*/
static SERIAL_TYPE   serial                  = { 0U };
static QueueHandle_t pSERIALqueue            = NULL;
static osThreadId_t  serialTxTaskHandle      = NULL;
static osThreadId_t  serialRxTaskHandle      = NULL;
static osThreadId_t  serialProtectTaskHandle = NULL;
/*------------------------ Variables ----------------------------------------------------------------*/
#if defined ( UNIT_TEST )
  static char     unitOutput[UNIT_TEST_BUFFER_SIZE] __section( TASK_RAM_SECTION ) = { 0U };
  static uint16_t unitCounter                       __section( TASK_RAM_SECTION ) = 0U;
  static uint8_t  unitSenderDone                    __section( TASK_RAM_SECTION ) = 0U;
#endif
/*---------------------------------------------------------------------------------------------------*/
osThreadId_t* osSERIALgetSerialTxTaskHandle ( void )
{
  return &serialTxTaskHandle;
}
osThreadId_t* osSERIALgetSerialRxTaskHandle ( void )
{
  return &serialRxTaskHandle;
}
osThreadId_t* osSERIALgetSerialProtectTaskHandle ( void )
{
  return &serialProtectTaskHandle;
}
QueueHandle_t* pSERIALgetQueue ( void )
{
  return &pSERIALqueue;
}
/*---------------------------------------------------------------------------------------------------*/
void vSERIALstartReading ( void )
{
  HAL_UART_Receive_IT( serial.uart, &serial.buffer, 1U );
}
/*---------------------------------------------------------------------------------------------------*/
uint8_t iSERIALisEndChar ( uint8_t input )
{
  uint8_t res = 0U;
  if ( ( input == SERIAL_END_CHAR_0 ) || ( input == SERIAL_END_CHAR_1 ) )
  {
    res = 1U;
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
/*
 * Send debug message thru serial
 * input: msg - string with message
 * output:  none
 */
void vSYSserial ( const char* data, uint16_t length )
{
  if ( ( serial.uart != NULL ) && ( pSERIALqueue != NULL ) && ( length > 0U ) )
  {
    const UART_MESSAGE message = {
      .data   = ( char* )data,
      .length = length
    };
    xQueueSend( pSERIALqueue, &message, SERIAL_OUTPUT_TIMEOUT );
  }
  return;
}
void vSYSserialString ( const char* string )
{
  vSYSserial( string, strlen( string ) );
  return;
}
#if defined ( UNIT_TEST )
void vUNITputChar ( int data )
{
  if ( unitCounter < UNIT_TEST_BUFFER_SIZE )
  {
    unitOutput[unitCounter++] = ( char )( data );
  }
  return;
}
void vUNITresetOutput ( void )
{
  unitCounter = 0U;
  return;
}
uint8_t eSERIALgetSerialState ( void )
{
  return unitSenderDone;
}
void vUNITwriteOutput ( void )
{
  vSYSserial( unitOutput, unitCounter );
  unitSenderDone = 0U;
  unitCounter    = 0U;
  return;
}
#endif
/*---------------------------------------------------------------------------------------------------*/
void vSERIALtxTask ( void* argument )
{
  for (;;)
  {
    if ( ulTaskNotifyTake( pdTRUE, SERIAL_OUTPUT_TIMEOUT ) > 0U )
    {
      if ( ( serial.state == SERIAL_STATE_READING ) && ( serial.error == 0U ) )
      {
        ( void )vCLIprocess( ( const char* )serial.input, serial.length );
        vSYSserialString( ( const char* )cCLIgetOutput() );
      }
      #if defined ( UNIT_TEST )
        unitSenderDone = 1U;
      #endif
      serial.length  = 0U;
      serial.state   = SERIAL_STATE_IDLE;
      serial.error   = 0U;
      vSERIALstartReading();
    }
  }
  return;
}
/*---------------------------------------------------------------------------------------------------*/
void vSERIALrxTask ( void* argument )
{
  for (;;)
  {
    if ( xQueueReceive( pSERIALqueue, &serial.output, SERIAL_OUTPUT_TIMEOUT ) == pdPASS )
    {
      if ( serial.output.length > 0U )
      {
        while ( serial.state != SERIAL_STATE_IDLE )
        {
          osDelay( SERIAL_OUTPUT_TIMEOUT );
        }
        serial.state = SERIAL_STATE_WRITING;
        ( void )HAL_UART_AbortReceive_IT( serial.uart );
        ( void )HAL_UART_Transmit_IT( serial.uart, ( uint8_t* )serial.output.data, serial.output.length );
      }
    }
  }
  return;
}
/*---------------------------------------------------------------------------------------------------*/
void vSERIALprotectTask ( void* argument )
{
  BaseType_t yield = pdFALSE;
  for (;;)
  {
    if ( ulTaskNotifyTake( pdTRUE, SERIAL_OUTPUT_TIMEOUT ) )
    {
      osDelay( SERIAL_PROTECT_TIMEOUT );
      if ( ( serial.state == SERIAL_STATE_READING ) && ( serialTxTaskHandle != NULL ) )
      {
        serial.error = 1U;
        vTaskNotifyGiveFromISR( ( TaskHandle_t )serialTxTaskHandle, &yield );
        portYIELD_FROM_ISR ( yield );
      }
    }
  }
  return;
}
/*---------------------------------------------------------------------------------------------------*/
/* Tx Transfer completed callbacks */
void HAL_UART_TxCpltCallback ( UART_HandleTypeDef *huart )
{
  BaseType_t yield = pdFALSE;
  if ( ( huart == serial.uart ) && ( serialTxTaskHandle != NULL ) )
  {
    vTaskNotifyGiveFromISR( ( TaskHandle_t )serialTxTaskHandle, &yield );
    portYIELD_FROM_ISR ( yield );
  }
  return;
}
/*---------------------------------------------------------------------------------------------------*/
/* Rx Transfer completed callbacks */
void HAL_UART_RxCpltCallback ( UART_HandleTypeDef *huart )
{
  BaseType_t yield = pdFALSE;
  if ( ( huart == serial.uart ) && ( serialProtectTaskHandle != NULL ) && ( serialTxTaskHandle != NULL ) )
  {
    switch ( serial.state )
    {
      case SERIAL_STATE_IDLE:
        serial.length    = 1U;
        serial.input[0U] = serial.buffer;
        serial.state     = SERIAL_STATE_READING;
        vSERIALstartReading();
        vTaskNotifyGiveFromISR( ( TaskHandle_t )serialProtectTaskHandle, &yield );
        portYIELD_FROM_ISR ( yield );
        break;
      case SERIAL_STATE_READING:
        serial.input[serial.length] = serial.buffer;
        serial.length++;
        if ( iSERIALisEndChar( serial.buffer ) > 0U )
        {
          serial.input[serial.length] = 0U;
          vTaskNotifyGiveFromISR( ( TaskHandle_t )serialTxTaskHandle, &yield );
          portYIELD_FROM_ISR ( yield );
        }
        else
        {
          vSERIALstartReading();
        }
        break;
      default:
        vTaskNotifyGiveFromISR( ( TaskHandle_t )serialTxTaskHandle, &yield );
        portYIELD_FROM_ISR ( yield );
        break;
    }
  }
  return;
}
/*---------------------------------------------------------------------------------------------------*/
/*
 * Declare UART for debug serial port
 * input:	uart - UART structure
 * output:	none
 */
void vSERIALinit ( UART_HandleTypeDef* uart )
{
  serial.uart  = uart;
  serial.state = SERIAL_STATE_IDLE;
  return;
}
/*---------------------------------------------------------------------------------------------------*/
uint32_t uSYSputChar ( char* str, uint32_t length, char ch )
{
  str[length] = ch;
  return ( length + 1U );
}
/*---------------------------------------------------------------------------------------------------*/
uint32_t uSYSendString ( char* str, uint32_t length )
{
  str[length]      = '\n';
  str[length + 1U] = 0U;
  return ( length + 1U );
}
/*---------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/


