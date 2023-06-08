/*
 * pdm_input.h
 *
 *  Created on: Jul 11, 2022
 *      Author: igor.dymov
 */

#ifndef PDM_INPUT_H_
#define PDM_INPUT_H_


#include "main.h"
#include "FreeRTOS.h"
#include "cmsis_os.h"
#include "event_groups.h"
#include "luatask.h"
#include "system.h"
#include "task.h"
#include "semphr.h"
#include "stm32f4xx_hal.h"
#include "platform_init.h"

//Количество дискретных входов, максимально 32 шт. из-за разрядности регистра для передачи данных
#ifdef PDM
	#define DIN_CHANNEL 11U
#endif
#ifdef PCM
	#define DIN_CHANNEL 20U
#endif

#define DIN_VALID   3U
#define MAX_VAL 65000U
#define DEF_H_FRONT 10U
#define DEF_L_FRONT 10U
#define CC_MAX  6

typedef enum {
	DIN_CONFIG_NEGATIVE = 0U,
	DIN_CONFIG_POSITIVE = 1U,
	RPM_CONFIG = 2U
} PDM_INPUT_TYPE;

typedef struct {
	uint32_t Pin;
	GPIO_TypeDef * GPIOx;
} PIN_CONFIG;

typedef enum {
		INPUT_1 = 0U,
		INPUT_2 = 1U,
		INPUT_3 = 2U,
		INPUT_4 = 3U,
		INPUT_5 = 4U,
		INPUT_6 = 5U,
		INPUT_7 = 6U,
		INPUT_8 = 7U,
		INPUT_9 = 8U,
		INPUT_10 = 9U,
		INPUT_11 = 10U,
		INPUT_12 = 11U,
		INPUT_13 = 12U,
		INPUT_14 = 13U,
		INPUT_15 = 14U,
		INPUT_16 = 15U,
		INPUT_17 = 16U,
		INPUT_18 = 17U,
		INPUT_19 = 18U,
		INPUT_20 = 19U
} DIN_INPUT_NAME;

typedef enum  {
		CONFIG_OK = 0,
		CONFIG_NO_SUPPORT = 1U,
		WRONG_CHANNEL_NUMBER =2U,
		WRONG_CHANNEL_MODE =3U,
} PDM_INPUT_CONFIG_ERROR;

typedef struct DinConfigDef_t
{
uint32_t      ulCounter;
uint32_t 	  ulLowCounter;
uint32_t 	  ulHighCounter;
uint8_t 	  ucValue;
uint8_t 	  ucTempValue;
PDM_INPUT_TYPE eInputType;
} DinConfig_t;


typedef struct RPMConfigDef_t
{
	uint32_t uiData;
	uint32_t uiRawData[CC_MAX ];
	uint8_t  ucCounter;
	uint8_t  ucValid;
	uint16_t usValidCounter;
} RPMConfig_t;

uint16_t uGetRPM1();
uint16_t uGetRPM2();
uint32_t uiGetDinMask();
void vGetCCData(uint8_t TimInd);
void vDinInit( void );
void vDinTask(void *argument);
PDM_INPUT_CONFIG_ERROR eDinConfig( uint8_t ucCh, PDM_INPUT_TYPE inType ,uint32_t ulHFront, uint32_t ulLFront);
void vSystemDinTimer(void);
uint8_t ucDinGet(DIN_INPUT_NAME channel);
#endif /* PDM_INPUT_H_ */
