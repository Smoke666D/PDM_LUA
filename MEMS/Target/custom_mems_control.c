/**
  ******************************************************************************
  * @file    custom_mems_control.c
  * @author  MEMS Application Team
  * @brief   This file contains the MEMS sensors interface for custom board
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

#include "custom_mems_conf.h"
#include "custom_mems_conf_app.h"
#include "custom_mems_control.h"

/**
  * @brief  Initializes accelerometer
  * @param  None
  * @retval None
  */
int32_t BSP_SENSOR_ACC_Init(void)
{
  return ( CUSTOM_MOTION_SENSOR_Init(CUSTOM_ACC_INSTANCE_0, MOTION_ACCELERO) );
}

/**
  * @brief  Initializes gyroscope
  * @param  None
  * @retval None
  */
void BSP_SENSOR_GYR_Init(void)
{
  (void)CUSTOM_MOTION_SENSOR_Init(CUSTOM_GYR_INSTANCE_0, MOTION_GYRO);
}
/**
  * @brief  Enables accelerometer
  * @param  None
  * @retval None
  */
void BSP_SENSOR_ACC_Enable(void)
{
  (void)CUSTOM_MOTION_SENSOR_Enable(CUSTOM_ACC_INSTANCE_0, MOTION_ACCELERO);
}

/**
  * @brief  Enables gyroscope
  * @param  None
  * @retval None
  */
void BSP_SENSOR_GYR_Enable(void)
{
  (void)CUSTOM_MOTION_SENSOR_Enable(CUSTOM_GYR_INSTANCE_0, MOTION_GYRO);
}


/**
  * @brief  Disables accelerometer
  * @param  None
  * @retval None
  */
void BSP_SENSOR_ACC_Disable(void)
{
  (void)CUSTOM_MOTION_SENSOR_Disable(CUSTOM_ACC_INSTANCE_0, MOTION_ACCELERO);
}

/**
  * @brief  Disables gyroscope
  * @param  None
  * @retval None
  */
void BSP_SENSOR_GYR_Disable(void)
{
  (void)CUSTOM_MOTION_SENSOR_Disable(CUSTOM_GYR_INSTANCE_0, MOTION_GYRO);
}


/**
  * @brief  Get accelerometer data
  * @param  Axes pointer to axes data structure
  * @retval None
  */
void BSP_SENSOR_ACC_GetAxes(MOTION_SENSOR_Axes_t *Axes)
{
  (void)CUSTOM_MOTION_SENSOR_GetAxes(CUSTOM_ACC_INSTANCE_0, MOTION_ACCELERO, (CUSTOM_MOTION_SENSOR_Axes_t *)Axes);
}

/**
  * @brief  Get gyroscope data
  * @param  Axes pointer to axes data structure
  * @retval None
  */
void BSP_SENSOR_GYR_GetAxes(MOTION_SENSOR_Axes_t *Axes)
{
#if (defined BSP_MOTION_SENSORS)
  #ifdef CUSTOM_GYR_INSTANCE_0
  (void)CUSTOM_MOTION_SENSOR_GetAxes(CUSTOM_GYR_INSTANCE_0, MOTION_GYRO, (CUSTOM_MOTION_SENSOR_Axes_t *)Axes);
  #else
  Axes->x = 0;
  Axes->y = 0;
  Axes->z = 0;
  #endif
#endif
}


/**
  * @brief  Set output data rate for accelerometer
  * @param  Odr Output Data Rate value to be set
  * @retval None
  */
void BSP_SENSOR_ACC_SetOutputDataRate(float Odr)
{
  (void)CUSTOM_MOTION_SENSOR_SetOutputDataRate(CUSTOM_ACC_INSTANCE_0, MOTION_ACCELERO, Odr);
}

/**
  * @brief  Set output data rate for gyroscope
  * @param  Odr Output Data Rate value to be set
  * @retval None
  */
void BSP_SENSOR_GYR_SetOutputDataRate(float Odr)
{
  (void)CUSTOM_MOTION_SENSOR_SetOutputDataRate(CUSTOM_GYR_INSTANCE_0, MOTION_GYRO, Odr);
}


/**
  * @brief  Get output data rate for accelerometer
  * @param  Odr Output Data Rate value
  * @retval None
  */
void BSP_SENSOR_ACC_GetOutputDataRate(float *Odr)
{
  (void)CUSTOM_MOTION_SENSOR_GetOutputDataRate(CUSTOM_ACC_INSTANCE_0, MOTION_ACCELERO, Odr);

}

/**
  * @brief  Get output data rate for gyroscope
  * @param  Odr Output Data Rate value
  * @retval None
  */
void BSP_SENSOR_GYR_GetOutputDataRate(float *Odr)
{
#if (defined BSP_MOTION_SENSORS)
  #ifdef CUSTOM_GYR_INSTANCE_0
  (void)CUSTOM_MOTION_SENSOR_GetOutputDataRate(CUSTOM_GYR_INSTANCE_0, MOTION_GYRO, Odr);
  #endif
#endif
}


/**
  * @brief  Set full scale for acclerometer
  * @param  Fullscale Fullscale value to be set
  * @retval None
  */
void BSP_SENSOR_ACC_SetFullScale(int32_t Fullscale)
{

#if (defined BSP_MOTION_SENSORS)
  #ifdef CUSTOM_ACC_INSTANCE_0
  (void)CUSTOM_MOTION_SENSOR_SetFullScale(CUSTOM_ACC_INSTANCE_0, MOTION_ACCELERO, Fullscale);
  #endif
#endif
}

/**
  * @brief  Set full scale for gyroscope
  * @param  Fullscale Fullscale value to be set
  * @retval None
  */
void BSP_SENSOR_GYR_SetFullScale(int32_t Fullscale)
{
#if (defined BSP_MOTION_SENSORS)
  #ifdef CUSTOM_GYR_INSTANCE_0
  (void)CUSTOM_MOTION_SENSOR_SetFullScale(CUSTOM_GYR_INSTANCE_0, MOTION_GYRO, Fullscale);
  #endif
#endif
}

/**
  * @brief  Get full scale for acclerometer
  * @param  Fullscale Fullscale value
  * @retval None
  */
void BSP_SENSOR_ACC_GetFullScale(int32_t *Fullscale)
{

#if (defined BSP_MOTION_SENSORS)
  #ifdef CUSTOM_ACC_INSTANCE_0
  (void)CUSTOM_MOTION_SENSOR_GetFullScale(CUSTOM_ACC_INSTANCE_0, MOTION_ACCELERO, Fullscale);
  #endif
#endif
}

/**
  * @brief  Get full scale for gyroscope
  * @param  Fullscale Fullscale value
  * @retval None
  */
void BSP_SENSOR_GYR_GetFullScale(int32_t *Fullscale)
{
#if (defined BSP_MOTION_SENSORS)
  #ifdef CUSTOM_GYR_INSTANCE_0
  (void)CUSTOM_MOTION_SENSOR_GetFullScale(CUSTOM_GYR_INSTANCE_0, MOTION_GYRO, Fullscale);
  #endif
#endif
}

