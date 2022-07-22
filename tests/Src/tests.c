/*
 * tests.c
 *
 *  Created on: Jul 20, 2022
 *      Author: 79110
 */
#include "tests.h"
#include "unity.h"
#include "cmsis_os.h"
#include "FreeRTOS.h"
#include "task.h"
#include "serial.h"

static osThreadId_t testHandle = NULL;

void vTESTsendReport ( void )
{
  vUNITwriteOutput();
  while ( eSERIALgetSerialState() == 0U )
  {
    osDelay( 10U );
  }
  return;
}

void vTESTtask ( void *argument )
{
  uint8_t done = 0U;
  for (;;)
  {
    if ( done == 0U )
    {
      UnityBegin( "PDM unit tests:" );
      runTest_flash();
      UnityEnd();
      done = 1U;
    }
    osDelay( 1000U );
  }
  return;
}

void vTESTinit ( void )
{
  const osThreadAttr_t tetsTask_attributes = {
    .name       = "UnitTestTask",
    .priority   = ( osPriority_t ) osPriorityNormal,
    .stack_size = 5120U
  };
  testHandle = osThreadNew( vTESTtask, NULL, &tetsTask_attributes );
  return;
}


