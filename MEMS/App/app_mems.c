/**
  ******************************************************************************
  * File Name          : app_mems.c
  * Description        : This file provides code for the configuration
  *                      of the STMicroelectronics.X-CUBE-MEMS1.9.3.0 instances.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "app_mems.h"
#include "main.h"
#include <stdio.h>

#include "stm32f4xx_hal.h"
#include "custom.h"
#include "bsp_ip_conf.h"
#include "fw_version.h"
#include "motion_di_manager.h"
#include "system.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define DWT_LAR_KEY  0xC5ACCE55 /* DWT register unlock key */
#define ALGO_FREQ  100U /* Algorithm frequency 100Hz */
#define ACC_ODR  ((float)ALGO_FREQ)
#define ACC_FS  1 /* FS = <-2g, 2g> */
#define ALGO_PERIOD  (1000000U / ALGO_FREQ) /* Algorithm period [us] */
#define FROM_MG_TO_G  0.001f
#define FROM_G_TO_MG  1000.0f
#define FROM_MDPS_TO_DPS  0.001f
#define FROM_DPS_TO_MDPS  1000.0f
#define FROM_MGAUSS_TO_UT50  (0.1f/50.0f)
#define FROM_UT50_TO_MGAUSS  500.0f

extern TIM_HandleTypeDef htim7;

/* Public variables ----------------------------------------------------------*/
uint32_t AlgoFreq = ALGO_FREQ;
MDI_cal_type_t AccCalMode = MDI_CAL_NONE;
MDI_cal_type_t GyrCalMode = MDI_CAL_NONE;

/* Extern variables ----------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static MOTION_SENSOR_Axes_t AccValue __SECTION(RAM_SECTION_CCMRAM);
static MOTION_SENSOR_Axes_t GyrValue __SECTION(RAM_SECTION_CCMRAM);
static int64_t Timestamp = 0;

/* Private function prototypes -----------------------------------------------*/
static void MEMS_INT1_Force_Low(void);
static void MEMS_INT1_Init(void);


static uint32_t ulRestartTimer()
{
	uint32_t data = htim7.Instance->CNT;
	htim7.Instance->CNT = 0U;
	return ( data );
}


void MX_MEMS_Init(void)
{
	  /* Force MEMS INT1 pin of the sensor low during startup in order to disable I3C and enable I2C. This function needs
	   * to be called only if user wants to disable I3C / enable I2C and didn't put the pull-down resistor to MEMS INT1 pin
	   * on his HW setup. This is also the case of usage X-NUCLEO-IKS01A2 or X-NUCLEO-IKS01A3 expansion board together with
	   * sensor in DIL24 adapter board where the LDO with internal pull-up is used.
	   */
	  MEMS_INT1_Force_Low();
	  /* Initialize (disabled) sensors */

	  BSP_SENSOR_ACC_Init();
	  BSP_SENSOR_GYR_Init();
	  BSP_SENSOR_ACC_SetOutputDataRate(ACC_ODR);
	  BSP_SENSOR_ACC_SetFullScale(ACC_FS);

	  /* Initialize MEMS INT1 pin back to it's default state after I3C disable / I2C enable */
	  MEMS_INT1_Init();
	  /* DynamicInclinometer API initialization function */
	  MotionDI_manager_init((int)ALGO_FREQ);
	  BSP_SENSOR_ACC_Enable();
	  BSP_SENSOR_GYR_Enable();
}


float YAW = 0;
float PITCH = 0;
float ROLL = 0;

float fAngleGet1 ( ANGLE_TYPE type )
{
	float data = 0.0f;
	switch(type)
	{
	case ANGLE_TYPE_ROLL:
		data = ROLL;
		break;
	case ANGLE_TYPE_PITCH:
		data = PITCH;
		break;
	default:
		break;
	}
  return (data);
}

/*
 * LM background task
 */
void MX_MEMS_Process(void)
{
	MDI_input_t      data_in;
	MDI_output_t     data_out;
	MDI_cal_type_t   acc_cal_mode;
	MDI_cal_type_t   gyro_cal_mode;
	MDI_cal_output_t acc_cal;
	MDI_cal_output_t gyro_cal;

	BSP_SENSOR_ACC_GetAxes(&AccValue);
	BSP_SENSOR_GYR_GetAxes(&GyrValue);

	/* Convert acceleration from [mg] to [g] */
	data_in.Acc[0] = (float)AccValue.x * FROM_MG_TO_G;
	data_in.Acc[1] = (float)AccValue.y * FROM_MG_TO_G;
	data_in.Acc[2] = (float)AccValue.z * FROM_MG_TO_G;

	/* Convert angular velocity from [mdps] to [dps] */
	data_in.Gyro[0] = (float)GyrValue.x * FROM_MDPS_TO_DPS;
	data_in.Gyro[1] = (float)GyrValue.y * FROM_MDPS_TO_DPS;
	data_in.Gyro[2] = (float)GyrValue.z * FROM_MDPS_TO_DPS;

	data_in.Timestamp = Timestamp;
	Timestamp += ulRestartTimer()*10;// ALGO_PERIOD;

	/* Run Dynamic Inclinometer algorithm */

	MotionDI_manager_run(&data_in, &data_out);



	/* Check calibration mode */
	MotionDI_get_acc_calibration_mode(&acc_cal_mode);
	MotionDI_get_gyro_calibration_mode(&gyro_cal_mode);

	if (acc_cal_mode != AccCalMode)
	{
	   AccCalMode = acc_cal_mode;
	}

	if (gyro_cal_mode != GyrCalMode)
	{
	  GyrCalMode = gyro_cal_mode;
	}

	/* Get calibration parameters */
	MotionDI_get_acc_calibration(&acc_cal);
	MotionDI_get_gyro_calibration(&gyro_cal);

	/* Convert accelerometer calibration parameters from [g] to [mg] */
	acc_cal.Bias[0] *= FROM_G_TO_MG;
	acc_cal.Bias[1] *= FROM_G_TO_MG;
	acc_cal.Bias[2] *= FROM_G_TO_MG;

	/* Convert gyroscope calibration parameters from [dps] to [mdps] */
	gyro_cal.Bias[0] *= FROM_DPS_TO_MDPS;
	gyro_cal.Bias[1] *= FROM_DPS_TO_MDPS;
	gyro_cal.Bias[2] *= FROM_DPS_TO_MDPS;
	ROLL = data_out.rotation[1];
	PITCH = data_out.rotation[2];
	YAW = data_out.rotation[0];
}



static void MEMS_INT1_Force_Low(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  GPIO_InitStruct.Pin = BSP_IP_MEMS_INT1_PIN_NUM;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(BSP_IP_MEMS_INT1_GPIOX, &GPIO_InitStruct);

  HAL_GPIO_WritePin(BSP_IP_MEMS_INT1_GPIOX, BSP_IP_MEMS_INT1_PIN_NUM, GPIO_PIN_RESET);
}

/**
 * @brief  Configure MEMS INT1 pin to the default state
 * @param  None
 * @retval None
 */
static void MEMS_INT1_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  GPIO_InitStruct.Pin = BSP_IP_MEMS_INT1_PIN_NUM;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(BSP_IP_MEMS_INT1_GPIOX, &GPIO_InitStruct);
}




#ifdef __cplusplus
}
#endif

