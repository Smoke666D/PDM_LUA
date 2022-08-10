/*
 * system.h
 *
 *  Created on: 8 нояб. 2021 г.
 *      Author: 79110
 */

#ifndef INC_SYSTEM_H_
#define INC_SYSTEM_H_
/*----------------------- Includes -------------------------------------*/
#include "stdint.h"
#include "cmsis_os2.h"
/*------------------------ Define --------------------------------------*/
#define  RAM_SECTION_RAM                ".ram"
#define  RAM_SECTION_CCMRAM             ".ccmram"
#define  TASK_RAM_SECTION                RAM_SECTION_CCMRAM

#define  DEFAULT_TASK_PRIORITY          osPriorityNormal
#define  LUA_TASK_PRIORITY              osPriorityLow
#define  ADC_TASK_PRIORITY              osPriorityHigh
#define  DOUT_TASK_PRIORITY             osPriorityHigh
#define  CAN_TASK_PRIORITY              osPriorityLow
#define  SERIAL_TX_TASK_PRIORITY        osPriorityLow
#define  SERIAL_RX_TASK_PRIORITY        osPriorityLow
#define  SERIAL_PROTECT_TASK_PRIORITY   osPriorityLow
#define  USB_TASK_PRIORITY              osPriorityLow
#define  DIN_TASK_PRIORITY              osPriorityLow

#define  DEFAULT_TASK_STACK_SIZE        128U
#define  LUA_TASK_STACK_SIZE            2000U
#define  ADC_TASK_STACK_SIZE            128U
#define  DOUT_TASK_STACK_SIZE           128U
#define  CAN_TASK_STACK_SIZE            128U
#define  DIN_TASK_STACK_SIZE            128U
#define  SERIAL_TX_TSAK_STACK_SIZE      256U
#define  SERIAL_RX_TSAK_STACK_SIZE      128U
#define  SERIAL_PROTECT_TSAK_STACK_SIZE 128U
#define  USB_TASK_STACK_SIZE            192U

#define  DEFAULT_TASK_NAME              "defaultTask"
#define  LUA_TASK_NAME                  "luaTask"
#define  ADC_TASK_NAME                  "adcTask"
#define  DOUT_TASK_NAME                 "doutTask"
#define  CAN_TASK_NAME                  "canTask"
#define  SERIAL_TX_TASK_NAME            "serialTxTask"
#define  SERIAL_RX_TASK_NAME            "serialRxTask"
#define  SERIAL_PROTECT_TASK_NAME       "serialProtectTask"
#define  USB_TASK_NAME                  "usbTask"
#define  DIN_TASK_NAME                  "dinTask"

#define  SYS_MAX_TSAK_NUMBER            20U
#define  SYS_BAR_LENGTH                 10U

#define  SERIAL_QUEUE_SIZE              16U
#define  CANRX_QUEUE_SIZE 				16U
#define  CANTX_QUEUE_SIZE 				16U
/*------------------------ Macros --------------------------------------*/
#ifndef   __SECTION
  #define __SECTION(x) __attribute__((section(x)))
#endif
/*------------------------ Enum ----------------------------------------*/
/*----------------------- Structures -----------------------------------*/
typedef struct
{
  osThreadId_t thread;
  uint32_t     size;
} TASK_ANALIZE;

typedef struct
{
  char*    name;
  uint16_t usage;
  uint16_t total;
} SYSTEM_DATA;
/*----------------------- Functions ------------------------------------*/
void    vSYSeventInit ( void );
void    vSYStaskInit ( void );
void    vSYSqueueInit ( void );
void    vSYSprintData ( void );
void    vSYSgetHeapData ( SYSTEM_DATA* data );
uint8_t uSYSgetTaskumber ( void );
void    vSYSgetTaskData ( uint8_t n, SYSTEM_DATA* data );
/*----------------------------------------------------------------------*/
#endif /* INC_SYSTEM_H_ */
