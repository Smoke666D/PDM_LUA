/*
 * common.h
 *
 *  Created on: 19 мар. 2020 г.
 *      Author: photo_Mickey
 */

#ifndef INC_SERIAL_H_
#define INC_SERIAL_H_
/*----------------------- Includes -------------------------------------*/
#include "stm32f4xx_hal.h"
#include "cmsis_os.h"
#include "FreeRTOS.h"
#include "queue.h"
/*------------------------ Define --------------------------------------*/
#define     SERIAL_QUEUE_SIZE           16U
#define     SERIAL_BUFFER_SIZE          25U
#define     SERIAL_END_CHAR_0           '\n'
#define     SERIAL_END_CHAR_1           '\r'
#define     SERIAL_OUTPUT_TIMEOUT       ( ( TickType_t )1000U  )
#define     SERIAL_PROTECT_TIMEOUT      ( ( TickType_t )100U )
#if defined ( UNIT_TEST )
  #define UNIT_TEST_BUFFER_SIZE   4096U
#endif
/*------------------------------ Enum ----------------------------------------*/
typedef enum
{
  INIT_OK,
  INIT_FAIL,
} INIT_STATE;

typedef enum
{
  SERIAL_STATE_IDLE,
  SERIAL_STATE_READING,
  SERIAL_STATE_WRITING
} SERIAL_STATE;
/*----------------------- Structures -----------------------------------*/
typedef struct
{
  GPIO_TypeDef* port;
  uint16_t      pin;
} GPIO_TYPE;

typedef struct
{
  char*    data;
  uint16_t length;
} UART_MESSAGE;

typedef struct
{
  UART_HandleTypeDef* uart;
  SERIAL_STATE        state;
  UART_MESSAGE        output;
  uint8_t             error;
  uint8_t             buffer;
  uint8_t             input[SERIAL_BUFFER_SIZE];
  uint16_t            length;
} SERIAL_TYPE;
/*----------------------- Functions ------------------------------------*/
void           vSERIALinit ( UART_HandleTypeDef* uart );
void           vSYSserial ( const char* data, uint16_t length );
void           vSYSserialString ( const char* string );
uint32_t       uSYSputChar ( char* str, uint32_t length, char ch );
uint32_t       uSYSendString ( char* str, uint32_t length );
osThreadId_t*  osSERIALgetSerialTxTaskHandle ( void );
osThreadId_t*  osSERIALgetSerialRxTaskHandle ( void );
osThreadId_t*  osSERIALgetSerialProtectTaskHandle ( void );
QueueHandle_t* pSERIALgetQueue ( void );
/* Tasks */
void     vSERIALtxTask ( void* argument );
void     vSERIALrxTask ( void* argument );
void     vSERIALprotectTask ( void* argument );
/* For testing */
#if defined ( UNIT_TEST )
  void    vUNITputChar ( int data );
  void    vUNITwriteOutput ( void );
  void    vUNITresetOutput ( void );
  uint8_t eSERIALgetSerialState ( void );
#endif
/*----------------------------------------------------------------------*/
#endif /* INC_COMMON_H_ */