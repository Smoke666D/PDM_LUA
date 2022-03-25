/**
 * @file rtc.h
 * @date 2016-07-15
 *
 * NOTE:
 * This file is generated by DAVE. Any manual modification done to this file will be lost when the code is
 * regenerated.
 */
/**
 * @cond
 ***********************************************************************************************************************
 * RTC v4.1.12 Facilitates to generate real time clock and alarm
 *
 * Copyright (c) 2015-2016, Infineon Technologies AG
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,are permitted provided that the
 * following conditions are met:
 *
 *   Redistributions of source code must retain the above copyright notice, this list of conditions and the  following
 *   disclaimer.
 *
 *   Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the
 *   following disclaimer in the documentation and/or other materials provided with the distribution.
 *
 *   Neither the name of the copyright holders nor the names of its contributors may be used to endorse or promote
 *   products derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE  FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY,OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT  OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * To improve the quality of the software, users are encouraged to share modifications, enhancements or bug fixes
 * with Infineon Technologies AG (dave@infineon.com).
 ***********************************************************************************************************************
 *
 * Change History
 * --------------
 *
 * 2015-02-16:
 *     - Initial version<br>
 *
 * 2015-05-18:
 *     - LLD version check added
 * 2016-07-15:
 *     - Added APIs to set and get time and alarm in standard time format.<br>
 * @endcond
 *
 */

#ifndef RTC_H
#define RTC_H


/***********************************************************************************************************************
 * HEADER FILES
 **********************************************************************************************************************/
#include <xmc_scu.h>
#include <xmc_rtc.h>
#include "rtc_conf.h"
#include <DAVE_Common.h>

/***********************************************************************************************************************
 * MACROS
 **********************************************************************************************************************/
#if (!((XMC_LIB_MAJOR_VERSION == 2U) && \
       (XMC_LIB_MINOR_VERSION >= 0U) && \
       (XMC_LIB_PATCH_VERSION >= 0U)))
#error "RTC APP requires XMC Peripheral Library v2.0.0 or higher"
#endif

/***********************************************************************************************************************
 * ENUMS
 **********************************************************************************************************************/
/**
 * @ingroup RTC_enumerations
 * @{
 */

typedef enum RTC_STATUS
{
  RTC_STATUS_SUCCESS = 0U,    /**< APP initialization status success */
  RTC_STATUS_FAILURE = 1U     /**< APP initialization status failure */
} RTC_STATUS_t;

/**
 * This enumeration defines RTC Module Enable and Disable values.
 */

typedef enum RTC_START
{
  RTC_START_DISABLE = 0x0U, /**< Disable RTC start */
  RTC_START_ENABLE = 0x1U   /**< Enable RTC start */
} RTC_START_t;


/**
 * This enumeration defines RTC Module Interrupt Related Events Enable and
 * Disable values.
 */

typedef enum RTC_INT
{
  RTC_INT_PERIODIC_SEC_DISABLE = 0x0U,          /**< Timer Periodic Seconds Interrupt Disable */
  RTC_INT_PERIODIC_SEC_ENABLE = 0x1U,           /**< Timer Periodic Seconds Interrupt Enable */
  RTC_INT_PERIODIC_MIN_DISABLE = 0x0U,          /**< Timer Periodic Minutes Interrupt Disable */
  RTC_INT_PERIODIC_MIN_ENABLE = 0x1U,           /**< Timer Periodic Minutes Interrupt Enable */
  RTC_INT_PERIODIC_HOUR_DISABLE = 0x0U,         /**< Timer Periodic Hours Interrupt Disable */
  RTC_INT_PERIODIC_HOUR_ENABLE = 0x1U,          /**< Timer Periodic Hours Interrupt Enable */
  RTC_INT_PERIODIC_DAY_DISABLE = 0x0U,          /**< Timer Periodic Days Interrupt Disable */
  RTC_INT_PERIODIC_DAY_ENABLE = 0x1U,           /**< Timer Periodic Days Interrupt Enable */
  RTC_INT_PERIODIC_MONTH_DISABLE = 0x0U,        /**< Timer Periodic Month Interrupt Disable */
  RTC_INT_PERIODIC_MONTH_ENABLE = 0x1U,         /**< Timer Periodic Month Interrupt Enable */
  RTC_INT_PERIODIC_YEAR_DISABLE = 0x0U,         /**< Timer Periodic Years Interrupt Disable */
  RTC_INT_PERIODIC_YEAR_ENABLE = 0x1U,          /**< Timer Periodic Years Interrupt Enable */
  RTC_INT_ALARM_DISABLE = 0x0U,                 /**< Alarm Interrupt Disable */
  RTC_INT_ALARM_ENABLE = 0x1U                   /**< Alarm Interrupt Enable */
} RTC_INT_t;

/***********************************************************************************************************************
* DATA STRUCTURES
***********************************************************************************************************************/
/**
  * @ingroup RTC_datastructures
  * @{
  */

/**
 * RTC APP Handle structure. Stores configured parameters in APP UI.
 */

typedef struct RTC_CONFIG
{
 RTC_START_t start;                           /**< rtc start */

 RTC_INT_t periodic_sec_intr;                 /**< periodic seconds interrupt */
 RTC_INT_t periodic_min_intr;                 /**< periodic minutes interrupt */
 RTC_INT_t periodic_hour_intr;                /**< periodic hours interrupt */
 RTC_INT_t periodic_day_intr;                 /**< periodic days interrupt */
 RTC_INT_t periodic_month_intr;               /**< periodic months interrupt */
 RTC_INT_t periodic_year_intr;                /**< periodic years interrupt */

 RTC_INT_t alarm_intr;                        /**< alarm interrupt */

#if ((UC_FAMILY == XMC4) && (RTC_INTERRUPT_ENABLED == 1))
 GLOBAL_SCU_XMC4_EVENT_HANDLER_t pi_listener;            /**< periodic interrupt listener */
 GLOBAL_SCU_XMC4_EVENT_HANDLER_t ai_listener;            /**< alarm interrupt listener */
#endif

#if ((UC_FAMILY == XMC1) && (RTC_INTERRUPT_ENABLED == 1))
 GLOBAL_SCU_XMC1_EVENT_HANDLER_t pi_listener;            /**< periodic interrupt listener */
 GLOBAL_SCU_XMC1_EVENT_HANDLER_t ai_listener;            /**< alarm interrupt listener */
#endif
} RTC_CONFIG_t;

/**
 * Runtime RTC handler.
 */
typedef struct RealTimeClock
{
  const XMC_RTC_CONFIG_t *const time_alarm_config; /** Pointer to RTC LLD configuration data */
  const RTC_CONFIG_t *const config;                /** Pointer to configuration data */
  bool initialized;                                /** status for initialized or not */
} RTC_t;
/**
 *@}
 */

#ifdef __cplusplus
extern "C" {
#endif
/**********************************************************************************************************************
 * API PROTOTYPES
***********************************************************************************************************************/

/**
 * @ingroup RTC_apidoc
 * @{
 */

/**
 * @brief Get RTC APP version.
 * @return @ref DAVE_APP_VERSION_t APP version information (major, minor and patch number)
 *
 * \par<b>Description: </b><br>
 * The function can be used to check application software compatibility with a
 * specific version of the APP.
 *
 * @code
 *  #include <DAVE.h>
 *  #include <DAVE_common.h>
 *  int main(void)
 *  {
 *    DAVE_APP_VERSION_t version;
 *    DAVE_Init();
 *    version = RTC_GetAppVersion();
 *    if(version.major != 4U)
 *    {
 *    }
 *    return 0;
 *  }
 * @endcode<BR> </p>
 */
DAVE_APP_VERSION_t RTC_GetAppVersion(void);


 /**
 * @brief Initializes RTC module.
 * @param handle Constant pointer to RTC structure of type @ref RTC_t
 * @return @ref RTC_STATUS_t
 *
 * \par<b>Description:</b><br>
 * Configures the RTC module registers as per settings updated in
 * UI and enables the RTC module.
 *
 * <P ALIGN="LEFT"><B>Example:</B>
 * Pre-requisite: Instantiate RTC APP and generate code before copying below code snippet.
 * @code
 *  #include <DAVE.h>
 *  int main(void)
 *  {
 *    DAVE_Init(); // RTC_Init(&RTC_0) will be called within DAVE_Init()
 *    // ... infinite loop ...
 *    while(1)
 *    {}
 *  }
 * @endcode<BR> </p> 
 */
RTC_STATUS_t RTC_Init(RTC_t *const handle);

/**
 * @brief Returns the time in seconds since the epoch time(01/01/1970).
 * @param time_value Pointer to structure of type @ref time_t
 * @return @ref RTC_STATUS_t
 *
 * <P ALIGN="LEFT"><B>Example:</B>
 * Pre-requisite: Instantiate RTC APP and generate code before copying below code snippet.
 * @code 
 * #include <DAVE.h>
 * int main(void)
 * {
 *   RTC_STATUS_t status = RTC_STATUS_FAILURE;
 *   time_t Time_Sec;
 *   DAVE_Init();  // RTC_Init(&RTC_0) will be called within DAVE_Init()
 *   status = RTC_Time(&Time_Sec);
 *   if(status != RTC_STATUS_SUCCESS)
 *   {
 *	   //error
 *   }
 *   // ... infinite loop ...
 *   while(1)
 *   {}
 * }
 * @endcode<BR> </p> 
 */
RTC_STATUS_t RTC_Time(time_t* time_value) ;

/**
 * @brief Starts the RTC running
 * @return None
 *
 *\par<b>Description: </b><br>
 * Before verifying this API it is required to deselect "Start after initialization" checbox in UI.
 * Thereby this API shall be used to start the RTC module.
 * <P ALIGN="LEFT"><B>Example:</B>
 * Pre-requisite:
 * 1. Instantiate RTC APP <br>
 * 2. Unselect "Start after initialization checkbox in UI" <br>
 * 3. Generate code before copying below code snippet. <br>
 * @code
 * #include <DAVE.h>
 * int main(void)
 * {
 *   DAVE_Init();// RTC_Init(&RTC_0) will be called within DAVE_Init()
 *   RTC_Stop();
 *   if (XMC_RTC_IsRunning() == false)
 *   {
 *     RTC_Start();
 *   }
 *   // ... infinite loop ...
 *   while(1)
 *   {}
 * }
 * @endcode<BR> </p>
 */
__STATIC_INLINE void RTC_Start(void)
{
	XMC_RTC_Start();
}

/**
 * @brief Stops the RTC running
 * @return None
 *
 * <P ALIGN="LEFT"><B>Example:</B>
 * Pre-requisite: Instantiate RTC APP <br>
 * Enable periodic seconds event in "Interrupt Settings" tab. <br>
 * Generate code before copying below code snippet.
 *
 * @code
 * #include <DAVE.h>
 *
 * void Time_Handler(void)
 * {
 *    static uint32_t seconds = 0;
 *    seconds++;
 *
 *    if(seconds == 30)
 *    {
 *      RTC_Stop();
 *    }
 * }
 * int main(void)
 * {
 *   DAVE_Init(); // RTC_Init(&RTC_0) will be called within DAVE_Init()
 *   // ... infinite loop ...
 *   while(1)
 *   {}
 * }
 * @endcode<BR> </p>
 */
__STATIC_INLINE void RTC_Stop(void)
{
	XMC_RTC_Stop();
}

/**
 * @brief Sets the time
 * @param time Pointer to structure of type @ref XMC_RTC_TIME_t
 * @return @ref RTC_STATUS_t
 *
 * <P ALIGN="LEFT"><B>Example:</B>
 * Pre-requisite: Instantiate RTC APP and generate code before copying below code snippet.
 * @code
 * #include <DAVE.h>
 * XMC_RTC_TIME_t timeval =
 * {
 *    .seconds = 0U,
 *    .minutes = 0U,
 *    .hours = 0U,
 *    .days = 1U,
 *    .month = 1,
 *    .year = 1970U
 * };
 * int main(void)
 * {
 *   RTC_STATUS_t status = RTC_STATUS_FAILURE;
 *   DAVE_Init();
 *   RTC_Stop();
 *   status = RTC_SetTime(&timeval);
 *   if(status != RTC_STATUS_SUCCESS)
 *   {
 *	   //error
 *   }
 *   // ... infinite loop ...
 *   while(1)
 *   {}
 * }
 * @endcode<BR> </p>
 */
RTC_STATUS_t RTC_SetTime(XMC_RTC_TIME_t *time);
/**
 * @brief Sets the time in standard format
 * @param stdtime Pointer to a ::tm structure containing the time parameters seconds,
 *                minutes, hours, days, daysofweek, month, year(since 1900) and days in a
 *                year in standard format.
 * @return @ref RTC_STATUS_t
 *
 * <P ALIGN="LEFT"><B>Example:</B>
 * Pre-requisite: Instantiate RTC APP and generate code before copying below code snippet.
 * @code
 * #include <DAVE.h>
 * #include <time.h>
 * struct tm stdtimeval =
 * {
 *    .tm_sec = 0U,
 *    .tm_min = 0U,
 *    .tm_hour = 0U,
 *    .tm_mday = 1U,
 *    .tm_mon = 1,
 *    .tm_year = 116U
 *    .tm_wday = 6
 * };
 * int main(void)
 * {
 *   RTC_STATUS_t status = RTC_STATUS_FAILURE;
 *   DAVE_Init();
 *   RTC_Stop();
 *   status = RTC_SetStdTime(&stdtimeval);
 *   if(status != RTC_STATUS_SUCCESS)
 *   {
 *	   //error
 *   }
 *   // ... infinite loop ...
 *   while(1)
 *   {}
 * }
 * @endcode<BR> </p>
 */
RTC_STATUS_t RTC_SetStdTime(struct tm *stdtime);
/**
 * @brief Sets the alarm time
 * @param alarm Pointer to structure of type @ref XMC_RTC_ALARM_t
 * @return @ref RTC_STATUS_t
 *
 * <P ALIGN="LEFT"><B>Example:</B>
 * Pre-requisite: Instantiate RTC APP and generate code before copying below code snippet.
 * @code
 * #include <DAVE.h>
 * XMC_RTC_ALARM_t alarm =
 * {
 *    .seconds = 0U,
 *    .minutes = 1U,
 *    .hours = 0U,
 *    .days = 1U,
 *    .month = 1,
 *    .year = 1970U
 * };
 * int main(void)
 * {
 *   RTC_STATUS_t status = RTC_STATUS_FAILURE;
 *   DAVE_Init();
 *   RTC_Stop();
 *   status = RTC_SetAlarmTime(&alarm);
 *   if(status != RTC_STATUS_SUCCESS)
 *   {
 *	   //error
 *   }
 *   // ... infinite loop ...
 *   while(1)
 *   {}
 * }
 * @endcode<BR> </p>
 */
RTC_STATUS_t RTC_SetAlarmTime(XMC_RTC_ALARM_t *alarm);
/**
 * @brief Sets the alarm time in standard format
 * @param stdtime Pointer to a ::tm structure containing the time parameters alarm seconds,
 *                alarm minutes, alarm hours, alarm days, alarm daysofweek, alarm month,
 *                alarm year(since 1900) and alarm days in a year in standard format.
 * @return @ref RTC_STATUS_t
 *
 * <P ALIGN="LEFT"><B>Example:</B>
 * Pre-requisite: Instantiate RTC APP and generate code before copying below code snippet.
 * @code
 * #include <DAVE.h>
 * #include <time.h>
 * struct tm stdalarmtime =
 * {
 *    .tm_sec = 0U,
 *    .tm_min = 0U,
 *    .tm_hour = 0U,
 *    .tm_mday = 1U,
 *    .tm_mon = 1,
 *    .tm_year = 116U
 * };
 * int main(void)
 * {
 *   RTC_STATUS_t status = RTC_STATUS_FAILURE;
 *   DAVE_Init();
 *   RTC_Stop();
 *   status = RTC_SetAlarmStdTime(&stdalarmtime);
 *   if(status != RTC_STATUS_SUCCESS)
 *   {
 *	   //error
 *   }
 *   // ... infinite loop ...
 *   while(1)
 *   {}
 * }
 * @endcode<BR> </p>
 */
RTC_STATUS_t RTC_SetAlarmStdTime(struct tm *stdtime);
/**
 * @brief Gets the time
 * @param time Pointer to structure of type @ref XMC_RTC_TIME_t
 * @return None
 *
 * <P ALIGN="LEFT"><B>Example:</B>
 * Pre-requisite: Instantiate RTC APP and generate code before copying below code snippet.
 * @code
 *
 * #include <DAVE.h>
 * XMC_RTC_TIME_t timeval =
 * {
 *   .seconds = 0U,
 *   .minutes = 0U,
 *   .hours = 0U,
 *   .days = 1U,
 *   .month = 1,
 *   .year = 1970U
 * };
 * int main(void)
 * {
 *  RTC_STATUS_t status = RTC_STATUS_FAILURE;
 *  DAVE_Init();
 *  RTC_Stop();
 *  status = RTC_SetTime(&timeval);
 *  if(status == RTC_STATUS_SUCCESS)
 *  {
 *    RTC_GetTime(&timeval);
 *  }
 *  // ... infinite loop ...
 *  while(1)
 *  {}
 * }
 * @endcode<BR> </p>
 */
void RTC_GetTime(XMC_RTC_TIME_t *time);
/**
 * @brief Gets the time in standard format
 * @param stdtime Pointer to a constant ::tm structure containing the time parameters seconds,
 *                minutes, hours, days, daysofweek, month, year(since 1900) and days in a
 *                year in standard format.
 * @return None
 *
 * <P ALIGN="LEFT"><B>Example:</B>
 * Pre-requisite: Instantiate RTC APP and generate code before copying below code snippet.
 * @code
 *
 * #include <DAVE.h>
 * #include <time.h>
 * struct tm stdtimeval =
 * {
 *    .tm_sec = 0U,
 *    .tm_min = 0U,
 *    .tm_hour = 0U,
 *    .tm_mday = 1U,
 *    .tm_mon = 1,
 *    .tm_year = 116U
 *    .tm_wday = 6
 * };
 * int main(void)
 * {
 *  RTC_STATUS_t status = RTC_STATUS_FAILURE;
 *  DAVE_Init();
 *  RTC_Stop();
 *  status = RTC_SetStdTime(&stdtimeval);
 *  if(status == RTC_STATUS_SUCCESS)
 *  {
 *    RTC_GetStdTime(&stdtimeval);
 *  }
 *  // ... infinite loop ...
 *  while(1)
 *  {}
 * }
 * @endcode<BR> </p>
 */
void RTC_GetStdTime(struct tm *stdtime);
/**
 * @brief Gets the alarm time
 * @param time Pointer to structure of type @ref XMC_RTC_ALARM_t
 * @return None
 *
 * <P ALIGN="LEFT"><B>Example:</B>
 * Pre-requisite: Instantiate RTC APP and generate code before copying below code snippet.
 * @code
 * #include <DAVE.h>
 * XMC_RTC_ALARM_t alarm =
 * {
 *   .seconds = 0U,
 *   .minutes = 1U,
 *   .hours = 0U,
 *   .days = 1U,
 *   .month = 1,
 *   .year = 1970U
 * };
 * int main(void)
 * {
 *  RTC_STATUS_t status = RTC_STATUS_FAILURE;
 *  DAVE_Init();
 *  RTC_Stop();
 *  status = RTC_SetAlarmTime(&alarm);
 *  if(status == RTC_STATUS_SUCCESS)
 *  {
 *    RTC_GetAlarmTime(&alarm);
 *  }
 *   // ... infinite loop ...
 *   while(1)
 *   {}
 * }
 * @endcode<BR> </p>
 */
void RTC_GetAlarmTime(XMC_RTC_ALARM_t *alarm);
/**
 * @brief Gets the alarm time in standard format
 * @param stdtime Pointer to a constant ::tm structure containing the time parameters alarm seconds,
 *                alarm minutes, alarm hours, alarm days, alarm daysofweek, alarm month,
 *                alarm year(since 1900) and alarm days in a year in standard format.
 * @return None
 *
 * <P ALIGN="LEFT"><B>Example:</B>
 * Pre-requisite: Instantiate RTC APP and generate code before copying below code snippet.
 * @code
 *
 * #include <DAVE.h>
 * #include <time.h>
 * struct tm stdalarmtime =
 * {
 *    .tm_sec = 0U,
 *    .tm_min = 0U,
 *    .tm_hour = 0U,
 *    .tm_mday = 1U,
 *    .tm_mon = 1,
 *    .tm_year = 116U
 * };
 * int main(void)
 * {
 *  RTC_STATUS_t status = RTC_STATUS_FAILURE;
 *  DAVE_Init();
 *  RTC_Stop();
 *  status = RTC_SetAlarmStdTime(&stdalarmtime);
 *  if(status == RTC_STATUS_SUCCESS)
 *  {
 *    RTC_GetAlarmStdTime(&stdalarmtime);
 *  }
 *  // ... infinite loop ...
 *  while(1)
 *  {}
 * }
 * @endcode<BR> </p>
 */
void RTC_GetAlarmStdTime(struct tm *stdtime);
/**
 * @brief Gets the flag status
 * @param None
 * @return uint32_t flag status
 *
 * <P ALIGN="LEFT"><B>Example:</B>
 * Pre-requisite: Instantiate RTC APP and generate code before copying below code snippet.
 * @code
 * #include <DAVE.h>
 * int main(void)
 * {
 *   DAVE_Init();	// RTC_Init(&RTC_0); is called inside DAVE_Init()
 *   if(RTC_GetFlagStatus()& RTC_STSSR_SPSE_Msk) // check if the periodic seconds event has occurred.
 *   {
 *   }
 *   while(1)
 *   {}
 * }
 * @endcode<BR> </p>
 */
uint32_t RTC_GetFlagStatus(void);

/**
 *@}
 */

#include "rtc_extern.h"

#ifdef __cplusplus
}
#endif

#endif


