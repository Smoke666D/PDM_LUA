/*
 * pdm_input.c
 *
 *  Created on: Jul 11, 2022
 *      Author: igor.dymov
 */

#ifndef PDM_INPUT_C_
#define PDM_INPUT_C_

#include "pdm_input.h"


static uint16_t usSystemTimer					__SECTION(RAM_SECTION_CCMRAM) = 0;
static uint16_t usTimer 						__SECTION(RAM_SECTION_CCMRAM) = 0;
static uint8_t  ucOverload						__SECTION(RAM_SECTION_CCMRAM) = 0;
static uint8_t  ucTicTime 						__SECTION(RAM_SECTION_CCMRAM) = 0;
static DinConfig_t xDinConfig[DIN_CHANNEL]  	__SECTION(RAM_SECTION_CCMRAM);
static EventGroupHandle_t  * pxPDMstatusEvent	__SECTION(RAM_SECTION_CCMRAM) = NULL;
extern TIM_HandleTypeDef htim10;
const  PIN_CONFIG xDinPortConfig[DIN_CHANNEL]= {{Din1_Pin,GPIOE},
											    {Din2_Pin,GPIOE},
											    {Din3_Pin,GPIOE},
											    {Din4_Pin,GPIOE},
											    {Din5_Pin,GPIOE},
											    {Din6_Pin,GPIOE},
											    {Din7_Pin,GPIOE},
											    {Din8_Pin,GPIOB},
											    {Din9_Pin,GPIOB},
											    {Din10_Pin,GPIOF},
											    {Din11_Pin,GPIOF}};


/*
 *
 */
static uint16_t usDinGetTimer(void)
{
 uint16_t usRes = 0U;
 if ( ucOverload )
 {
	 ucOverload = 0U;
	 usRes = MAX_VAL - usTimer + usSystemTimer;
 }
 else
 {
	 usRes = usSystemTimer - usTimer;
 }
 usTimer = usSystemTimer;
 return ( usRes );
}
/*
 *
 */
void vSystemDinTimer(void)
{
	usSystemTimer ++;
	if (usSystemTimer  >= MAX_VAL)
	{
		usSystemTimer = 0U;
		ucOverload 	  = 1U;
	}
	return;
}


void vGetCCData(TIM_HandleTypeDef *htim)
{
	uint16_t apc = HAL_TIM_ReadCapturedValue(&htim10,TIM_CHANNEL_1);
	__HAL_TIM_SET_COUNTER(&htim10, 0x0000);
	//__HAL_TIM_CLEAR_FLAG(&htim10,TIM_FLAG_CC1);
}
/*
 *
 */
PDM_INPUT_CONFIG_ERROR eDinConfig( uint8_t ucCh, LOGIC_STATE eLogicState, PDM_INPUT_TYPE inType, uint32_t ulHFront, uint32_t ulLFront)
{
	PDM_INPUT_CONFIG_ERROR eRes = WRONG_CHANNEL_NUMBER;
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	if ( ucCh < DIN_CHANNEL)
	{
		xDinConfig[ucCh].eInputType =inType;
		if ( inType == DIN_CONGIG )
		{
			GPIO_InitStruct.Pin = xDinPortConfig[ucCh].Pin;
			GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
			GPIO_InitStruct.Pull = GPIO_PULLDOWN;
			HAL_GPIO_Init(xDinPortConfig[ucCh].GPIOx,&GPIO_InitStruct);
		}
		xDinConfig[ucCh].ulHighCounter = ulHFront;
		xDinConfig[ucCh].ulLowCounter = ulLFront;
		xDinConfig[ucCh].ucValue = (eLogicState == NEGATIVE_STATE) ?1U : 0U;
		xDinConfig[ucCh].ucTempValue = 0U;
		xDinConfig[ucCh].eState = eLogicState;
		eRes = CONFIG_OK;
	}
	return ( eRes );
}
/*
 *
 */
void vDinInit( void )
{
	eDinConfig(INPUT_1,POSITIVE_STATE, DIN_CONGIG, DEF_H_FRONT,DEF_L_FRONT );
	eDinConfig(INPUT_2,POSITIVE_STATE,DIN_CONGIG,DEF_H_FRONT,DEF_L_FRONT );
	eDinConfig(INPUT_3,POSITIVE_STATE,DIN_CONGIG,DEF_H_FRONT,DEF_L_FRONT );
	eDinConfig(INPUT_4,POSITIVE_STATE,DIN_CONGIG,DEF_H_FRONT,DEF_L_FRONT  );
	eDinConfig(INPUT_5,POSITIVE_STATE,DIN_CONGIG,DEF_H_FRONT,DEF_L_FRONT );
	eDinConfig(INPUT_6,POSITIVE_STATE,DIN_CONGIG,DEF_H_FRONT,DEF_L_FRONT  );
	eDinConfig(INPUT_7,POSITIVE_STATE,DIN_CONGIG,DEF_H_FRONT,DEF_L_FRONT );
	eDinConfig(INPUT_8,POSITIVE_STATE,DIN_CONGIG,DEF_H_FRONT,DEF_L_FRONT  );
	eDinConfig(INPUT_9,POSITIVE_STATE,RMP_OCNFIG,DEF_H_FRONT,DEF_L_FRONT  );
	eDinConfig(INPUT_10,POSITIVE_STATE,DIN_CONGIG,DEF_H_FRONT,DEF_L_FRONT  );
	eDinConfig(INPUT_11,POSITIVE_STATE,DIN_CONGIG, DEF_H_FRONT,DEF_L_FRONT  );
	return;
}
/*
 *
 */
void vDinTask(void *argument)
{
	uint32_t ulDinState;
	pxPDMstatusEvent = osLUAetPDMstatusHandle();
	HAL_TIM_IC_Start_IT(&htim10,TIM_CHANNEL_1);
	while(1)
	{
		vTaskDelay(1);
		xEventGroupWaitBits(* pxPDMstatusEvent, RUN_STATE, pdFALSE, pdTRUE, portMAX_DELAY );
		ucTicTime = usDinGetTimer();
		for (uint8_t i = 0U; i <DIN_CHANNEL; i++)
		{
				if ( xDinConfig[i].eInputType == DIN_CONGIG )
				{
					ulDinState = HAL_GPIO_ReadPin(xDinPortConfig[i].GPIOx,xDinPortConfig[i].Pin);
					if (ulDinState != xDinConfig[i].ucTempValue )
					{
						xDinConfig[i].ulCounter +=  ucTicTime ;
						if (xDinConfig[i].ulCounter > (xDinConfig[i].ucTempValue == GPIO_PIN_RESET) ? xDinConfig[i].ulHighCounter : xDinConfig[i].ulLowCounter )
						{
							if (ulDinState == GPIO_PIN_SET)
								xDinConfig[i].ucValue = (xDinConfig[i].eState == NEGATIVE_STATE) ?0U:1U;
							else
								xDinConfig[i].ucValue = (xDinConfig[i].eState == NEGATIVE_STATE) ?1U:0U;
							xDinConfig[i].ucTempValue = ulDinState ;
						}
					}
					else
					{
						xDinConfig[i].ulCounter = 0U;
					}
				}
				else
				{
					xDinConfig[i].ucValue = 1U;
				}

		}
	}
	return;
}
/*
 *
*/
uint8_t ucDinGet( PDM_INPUT_NAME eChNum )
{
	return ( (eChNum < DIN_CHANNEL) ? xDinConfig[ eChNum ].ucValue: 0U );
}

#endif /* PDM_INPUT_C_ */
