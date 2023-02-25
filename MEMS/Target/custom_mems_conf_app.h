/**
  ******************************************************************************
  * @file    custom_mems_conf_app.h
  * @author  MEMS Application Team
  * @brief   Custom sensor configuration file
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
#ifndef __CUSTOM_MEMS_CONF_APP_H__
#define __CUSTOM_MEMS_CONF_APP_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "sensor_unicleo_id.h"

/* Exported defines ----------------------------------------------------------*/
#ifdef USE_CUSTOM_MOTION_SENSOR_LSM6DSL_0
  #if (USE_CUSTOM_MOTION_SENSOR_LSM6DSL_0 == 1)
    #ifndef CUSTOM_ACC_INSTANCE_0
      #define CUSTOM_ACC_INSTANCE_0 CUSTOM_LSM6DSL_0
      #define CUSTOM_ACC_INSTANCE_NAME_0 "LSM6DSL"
      #define CUSTOM_ACC_INSTANCE_ODR_0 {10, 12.5, 26, 52, 104, 208, 416, 833, 1660, 3330, 6660} /* Hz */
      #define CUSTOM_ACC_INSTANCE_FS_0 {4, 2, 4, 8, 16} /* g */
      #ifndef ACC_GYR_UNICLEO_ID
        #define ACC_GYR_UNICLEO_ID LSM6DSL_UNICLEO_ID
      #endif
    #endif
    #ifndef CUSTOM_GYR_INSTANCE_0
      #define CUSTOM_GYR_INSTANCE_0 CUSTOM_LSM6DSL_0
      #define CUSTOM_GYR_INSTANCE_NAME_0 "LSM6DSL"
      #define CUSTOM_GYR_INSTANCE_ODR_0 {10, 12.5, 26, 52, 104, 208, 416, 833, 1660, 3330, 6660} /* Hz */
      #define CUSTOM_GYR_INSTANCE_FS_0 {5, 125, 250, 500, 1000, 2000} /* dps */

    #endif
  #endif
#endif



#ifdef __cplusplus
}
#endif

#endif /* __CUSTOM_MEMS_CONF_APP_H__ */
