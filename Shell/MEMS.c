/*
 * MEMS.c
 *
 *  Created on: Nov 11, 2022
 *      Author: igor.dymov
 */


#include "mems.h"
#include "motion_di.h"
#include "main.h"


float freq = 100.0f;
static MDI_knobs_t iKnobs;
static MDI_knobs_t *ipKnobs = &iKnobs;
static EventGroupHandle_t  * pxPDMstatusEvent	__SECTION(RAM_SECTION_CCMRAM);


//static MOTION_SENSOR_Axes_t AccValue;
//static MOTION_SENSOR_Axes_t GyrValue;

/*void BSP_SENSOR_ACC_GetOrientation(char *Orientation)
{
  Orientation[0] = 's';
  Orientation[1] = 'e';
  Orientation[2] = 'u';
}*/

/**
  * @brief  Get gyroscope sensor orientation
  * @param  Orientation Pointer to sensor orientation
  * @retval None
  */
/*void BSP_SENSOR_GYR_GetOrientation(char *Orientation)
{
  Orientation[0] = 's';
  Orientation[1] = 'e';
  Orientation[2] = 'u';
}*/

static void Init_Sensors(void)
{
	BSP_SENSOR_ACC_Init();
	  BSP_SENSOR_GYR_Init();
	  BSP_SENSOR_MAG_Init();
	  BSP_SENSOR_PRESS_Init();
	  BSP_SENSOR_TEMP_Init();
	  BSP_SENSOR_HUM_Init();

	  BSP_SENSOR_ACC_SetOutputDataRate(ACC_ODR);
	  BSP_SENSOR_ACC_SetFullScale(ACC_FS);
}

static void MX_DynamicInclinometer_Init(void)
{


  /* Initialize (disabled) sensors */

  Init_Sensors();
  /* DynamicInclinometer API initialization function */
  MotionDI_Initialize(&freq);
  MotionDI_getKnobs(ipKnobs);
  ipKnobs->AccKnob.CalType = MDI_CAL_CONTINUOUS;
  ipKnobs->GyrKnob.CalType = MDI_CAL_CONTINUOUS;
  BSP_SENSOR_ACC_GetOrientation(ipKnobs->AccOrientation);
  BSP_SENSOR_GYR_GetOrientation(ipKnobs->GyroOrientation);
  ipKnobs->SFKnob.output_type = MDI_ENGINE_OUTPUT_ENU;
  ipKnobs->SFKnob.modx = DECIMATION;
  MotionDI_setKnobs(ipKnobs);
}


static MDI_output_t data_out;
static MDI_input_t data_in;

void vmemsTask(void *argument)
{
	pxPDMstatusEvent = osLUAetPDMstatusHandle();
	MX_DynamicInclinometer_Init();
	data_in.Timestamp = 0;
	for(;;)
	{
	   osDelay(10);
	   xEventGroupWaitBits(* pxPDMstatusEvent, RUN_STATE, pdFALSE, pdTRUE, portMAX_DELAY );
	   data_in.Gyro[0] = 0.1;
	   data_in.Gyro[1] = 0.8;
	   data_in.Gyro[2] = 0.2;
	   data_in.Acc[0] = 40.0;
	   data_in.Acc[1] = 20.0;
	   data_in.Acc[2] = 10.0;
	   data_in.Timestamp +=10;
	   MotionDI_update(&data_out,&data_in);
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
