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

#define DIN_CHANNEL 11U
#define DIN_VALID   3U
#define MAX_VAL 65000U
#define DEF_H_FRONT 10U
#define DEF_L_FRONT 10U


typedef struct {
	uint32_t Pin;
	GPIO_TypeDef * GPIOx;
} PIN_CONFIG;

typedef enum {
	POSITIVE_STATE = 0U,
	NEGATIVE_STATE = 1U,
} LOGIC_STATE;

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
		INPUT_11 = 10U
} PDM_INPUT_NAME;

typedef enum  {
		CONFIG_OK = 0,
		CONFIG_NO_SUPPORT = 1U,
		WRONG_CHANNEL_NUMBER =2U,
		WRONG_CHANNEL_MODE =3U,
} PDM_INPUT_CONFIG_ERROR;

typedef struct DinConfigDef_t
{
uint32_t      ulCounter;
uint8_t 	  ucValue;
uint32_t 	  ulLowCounter;
uint32_t 	  ulHighCounter;
uint8_t 	  ucTempValue;
LOGIC_STATE   eState;
} DinConfig_t;


void vDinInit( void );
void vDinTask(void *argument);
PDM_INPUT_CONFIG_ERROR eDinConfig( uint8_t ucCh, LOGIC_STATE eLogicState, uint32_t ulHFront, uint32_t ulLFront);
void vSystemDinTimer(void);
uint8_t ucDinGet(PDM_INPUT_NAME channel);
#endif /* PDM_INPUT_H_ */
