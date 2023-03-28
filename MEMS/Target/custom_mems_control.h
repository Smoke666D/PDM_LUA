/**
  ******************************************************************************
  * @file    custom_mems_control.h
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

 /* Define to prevent recursive inclusion -------------------------------------*/
#ifndef CUSTOM_MEMS_CONTROL_H
#define CUSTOM_MEMS_CONTROL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "RTE_Components.h"

#if (defined BSP_MOTION_SENSORS)
#include "custom_motion_sensors.h"
#endif


typedef struct
{
  int32_t x;
  int32_t y;
  int32_t z;
} MOTION_SENSOR_Axes_t;

int32_t BSP_SENSOR_ACC_Init(void);
void BSP_SENSOR_GYR_Init(void);


void BSP_SENSOR_ACC_Enable(void);
void BSP_SENSOR_GYR_Enable(void);


void BSP_SENSOR_ACC_Disable(void);
void BSP_SENSOR_GYR_Disable(void);


void BSP_SENSOR_ACC_GetAxes(MOTION_SENSOR_Axes_t *Axes);
void BSP_SENSOR_GYR_GetAxes(MOTION_SENSOR_Axes_t *Axes);


void BSP_SENSOR_ACC_SetOutputDataRate(float Odr);
void BSP_SENSOR_GYR_SetOutputDataRate(float Odr);


void BSP_SENSOR_ACC_GetOutputDataRate(float *Odr);
void BSP_SENSOR_GYR_GetOutputDataRate(float *Odr);


void BSP_SENSOR_ACC_SetFullScale(int32_t Fullscale);
void BSP_SENSOR_GYR_SetFullScale(int32_t Fullscale);


void BSP_SENSOR_ACC_GetFullScale(int32_t *Fullscale);
void BSP_SENSOR_GYR_GetFullScale(int32_t *Fullscale);


#endif /* CUSTOM_MEMS_CONTROL_H */
