/*
 * MEMS.c
 *
 *  Created on: Nov 11, 2022
 *      Author: igor.dymov
 */


#include "mems.h"
#include "app_mems.h"
extern TIM_HandleTypeDef htim7;

static EventGroupHandle_t  * pxPDMstatusEvent	__SECTION(RAM_SECTION_CCMRAM);

float fAngleGet ( ANGLE_TYPE type )
{
	return  ( fAngleGet1 (  type ) );
}

void vmemsTask(void *argument)
{
	pxPDMstatusEvent = osLUAetPDMstatusHandle();
	HAL_TIM_Base_Start(&htim7);
	for(;;)
	{

	   vTaskDelay(10);
	   xEventGroupWaitBits(* pxPDMstatusEvent, RUN_STATE, pdFALSE, pdTRUE, portMAX_DELAY );
	   MX_MEMS_Process();
	}
}


