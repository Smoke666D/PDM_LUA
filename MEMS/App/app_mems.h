/**
  ******************************************************************************
  * File Name          : app_mems.h
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __APP_MEMS_H
#define __APP_MEMS_H

#ifdef __cplusplus
 extern "C" {
#endif
#include "PDMhardware.h"
/* Includes ------------------------------------------------------------------*/

/* Exported defines ----------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/
void MX_MEMS_Init(void);
void MX_MEMS_Process(void);
float fAngleGet1 ( ANGLE_TYPE type );
#ifdef __cplusplus
}
#endif

#endif /* __APP_MEMS_H */

