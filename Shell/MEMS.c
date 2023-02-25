/*
 * MEMS.c
 *
 *  Created on: Nov 11, 2022
 *      Author: igor.dymov
 */


#include "mems.h"
#include "motion_di.h"
#include "main.h"
#include "bsp_ip_conf.h"
#include "app_mems.h"

static EventGroupHandle_t  * pxPDMstatusEvent	__SECTION(RAM_SECTION_CCMRAM);


static MDI_output_t data_out;

void vmemsTask(void *argument)
{


	for(;;)
	{
	   osDelay(10);
	  // xEventGroupWaitBits(* pxPDMstatusEvent, RUN_STATE, pdFALSE, pdTRUE, portMAX_DELAY );
	   MX_MEMS_Process();
	}
}

float fAngleGet ( ANGLE_TYPE type )
{
	float data = 0.0f;
	switch(type)
	{
	case ANGLE_TYPE_ROLL:
		data = data_out.rotation[2];
		break;
	case ANGLE_TYPE_PITCH:
		data = data_out.rotation[1];
		break;

	}
  return (data);
}
