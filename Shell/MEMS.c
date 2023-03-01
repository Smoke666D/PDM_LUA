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

float fAngleGet ( ANGLE_TYPE type )
{
	return  fAngleGet1 (  type );
}

void vmemsTask(void *argument)
{
	pxPDMstatusEvent = osLUAetPDMstatusHandle();
	for(;;)
	{
	   osDelay(10);
	   xEventGroupWaitBits(* pxPDMstatusEvent, RUN_STATE, pdFALSE, pdTRUE, portMAX_DELAY );
	   MX_MEMS_Process();
	}
}


