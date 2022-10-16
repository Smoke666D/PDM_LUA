/*
 * pdm_input.c
 *
 *  Created on: Jul 11, 2022
 *      Author: igor.dymov
 */

#ifndef PDM_INPUT_C_
#define PDM_INPUT_C_

#include "pdm_input.h"


static uint16_t usSystemTimer					__SECTION(RAM_SECTION_CCMRAM);
static uint16_t usTimer 						__SECTION(RAM_SECTION_CCMRAM);
static uint8_t  ucOverload						__SECTION(RAM_SECTION_CCMRAM);
static uint8_t  ucTicTime 						__SECTION(RAM_SECTION_CCMRAM);
static DinConfig_t xDinConfig[DIN_CHANNEL]  	__SECTION(RAM_SECTION_CCMRAM);
static EventGroupHandle_t  * pxPDMstatusEvent	__SECTION(RAM_SECTION_CCMRAM);
static RPMConfig_t eRPM[2] 						__SECTION(RAM_SECTION_CCMRAM);


extern TIM_HandleTypeDef htim9;
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
/*
 *
 */
uint16_t uGetRPM1()
{
	uint16_t usTemp = 0;
	if (eRPM[0].uiData != 0)
	{
		usTemp =(uint16_t)(( 1.0/( (float)eRPM[0].uiData * 0.0002D) )* 60);
	}
	else
		usTemp = 0;
	return (xDinConfig[INPUT_9].eInputType==RPM_CONFIG) ? usTemp : 0U;

}
/*
 *
 */
uint16_t uGetRPM2()
{
	uint16_t usTemp = 0;
	if (eRPM[1].uiData != 0)
	{
		usTemp =(uint16_t)(( 1.0/( (float)eRPM[1].uiData * 0.0002D) )* 60);
	}
	else
		usTemp = 0;
	return ( (xDinConfig[INPUT_6].eInputType==RPM_CONFIG) ? usTemp : 0U );
}


/*
 *
 */
static void vCheckRPM( uint8_t index)
{
	eRPM[index].usValidCounter++;
	if (eRPM[index].usValidCounter ==1000)
	{
		eRPM[index].usValidCounter = 0;
		if (eRPM[index].ucValid)
		{
			eRPM[index].ucValid = 0;
		}
		else
		{
			eRPM[index].uiData	= 0;
		}
	}
	return;
}
/*
 *
 */
void vGetCCData(uint8_t TimInd)
{
	uint8_t ucTimIndex;
	switch (TimInd)
	{
		case 0:
			ucTimIndex = 0;
			eRPM[ucTimIndex].uiRawData[eRPM[ucTimIndex].ucCounter] = HAL_TIM_ReadCapturedValue(&htim10,TIM_CHANNEL_1);
			__HAL_TIM_SET_COUNTER(&htim10, 0x0000);
			break;
		case 1:
			ucTimIndex = 1;
			eRPM[ucTimIndex].uiRawData[eRPM[ucTimIndex].ucCounter] = HAL_TIM_ReadCapturedValue(&htim9,TIM_CHANNEL_1);
			__HAL_TIM_SET_COUNTER(&htim9, 0x0000);
			break;
		default:
			return;
	}
	eRPM[ucTimIndex].ucCounter ++;
	eRPM[ucTimIndex].ucValid = 1;
	if (eRPM[ucTimIndex].ucCounter == CC_MAX)
	{
		unsigned long ulTemp = 0;
		eRPM[ucTimIndex].ucCounter = 0;
		for (uint8_t i=0; i < CC_MAX; i++)
		{
			ulTemp += eRPM[ucTimIndex].uiRawData[i];
		}
		eRPM[ucTimIndex].uiData = (ulTemp/CC_MAX)*2;
	}
	return;
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
		if (inType == RPM_CONFIG)
		{
			switch (ucCh)
			{
				case INPUT_9:
					eRPM[0].ucCounter =0;
					eRPM[0].uiData = 0;
					eRPM[0].usValidCounter = 0;
					eRPM[0].ucValid = 0;
					__HAL_RCC_TIM10_CLK_ENABLE();
  				    __HAL_RCC_GPIOB_CLK_ENABLE();
					GPIO_InitStruct.Pin = Din9_Pin;
					GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
					GPIO_InitStruct.Pull = GPIO_NOPULL;
					GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
					GPIO_InitStruct.Alternate = GPIO_AF3_TIM10;
					HAL_GPIO_Init(Din9_GPIO_Port, &GPIO_InitStruct);
					/* TIM10 interrupt Init */
					HAL_NVIC_SetPriority(TIM1_UP_TIM10_IRQn, 5, 0);
					HAL_NVIC_EnableIRQ(TIM1_UP_TIM10_IRQn);
					HAL_TIM_IC_Start_IT(&htim10,TIM_CHANNEL_1);
					break;
				case INPUT_6:
					eRPM[1].ucCounter =0;
					eRPM[1].uiData = 0;
					eRPM[1].usValidCounter = 0;
					eRPM[1].ucValid = 0;
   				    __HAL_RCC_TIM9_CLK_ENABLE();
					__HAL_RCC_GPIOE_CLK_ENABLE();
					GPIO_InitStruct.Pin = Din6_Pin;
					GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
					GPIO_InitStruct.Pull = GPIO_NOPULL;
					GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
					GPIO_InitStruct.Alternate = GPIO_AF3_TIM9;
					HAL_GPIO_Init(Din6_GPIO_Port, &GPIO_InitStruct);
					/* TIM9 interrupt Init */
					HAL_NVIC_SetPriority(TIM1_BRK_TIM9_IRQn, 5, 0);
					HAL_NVIC_EnableIRQ(TIM1_BRK_TIM9_IRQn);
					HAL_TIM_IC_Start_IT(&htim9,TIM_CHANNEL_1);
					break;
				default:
					xDinConfig[ucCh].eInputType = DIN_CONFIG;
					break;
			}
		}
		if (xDinConfig[ucCh].eInputType == DIN_CONFIG )
		{
			GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
			GPIO_InitStruct.Pull = GPIO_PULLDOWN;
			GPIO_InitStruct.Pin = xDinPortConfig[ucCh].Pin;
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
	eDinConfig(INPUT_1,POSITIVE_STATE, DIN_CONFIG, DEF_H_FRONT,DEF_L_FRONT );
	eDinConfig(INPUT_2,POSITIVE_STATE,DIN_CONFIG,DEF_H_FRONT,DEF_L_FRONT );
	eDinConfig(INPUT_3,POSITIVE_STATE,DIN_CONFIG,DEF_H_FRONT,DEF_L_FRONT );
	eDinConfig(INPUT_4,POSITIVE_STATE,DIN_CONFIG,DEF_H_FRONT,DEF_L_FRONT  );
	eDinConfig(INPUT_5,POSITIVE_STATE,DIN_CONFIG,DEF_H_FRONT,DEF_L_FRONT );
	eDinConfig(INPUT_6,POSITIVE_STATE,DIN_CONFIG,DEF_H_FRONT,DEF_L_FRONT  );
	eDinConfig(INPUT_7,POSITIVE_STATE,DIN_CONFIG,DEF_H_FRONT,DEF_L_FRONT );
	eDinConfig(INPUT_8,POSITIVE_STATE,DIN_CONFIG,DEF_H_FRONT,DEF_L_FRONT  );
	eDinConfig(INPUT_9,POSITIVE_STATE,DIN_CONFIG,DEF_H_FRONT,DEF_L_FRONT  );
	eDinConfig(INPUT_10,POSITIVE_STATE,DIN_CONFIG,DEF_H_FRONT,DEF_L_FRONT  );
	eDinConfig(INPUT_11,POSITIVE_STATE,DIN_CONFIG, DEF_H_FRONT,DEF_L_FRONT  );
	return;
}
/*
 *
 */

void vDinTask(void *argument)
{
	uint32_t ulDinState;
	pxPDMstatusEvent = osLUAetPDMstatusHandle();
	while(1)
	{
		vTaskDelay(1);
		xEventGroupWaitBits(* pxPDMstatusEvent, RUN_STATE, pdFALSE, pdTRUE, portMAX_DELAY );
		ucTicTime = usDinGetTimer();
		for (uint8_t i = 0U; i <DIN_CHANNEL; i++)
		{
				if ( xDinConfig[i].eInputType == DIN_CONFIG )
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
					switch (i)
					{
						case INPUT_9:
							vCheckRPM(0);
						break;
						case INPUT_6:
							vCheckRPM(1);
							break;
						default:
							break;
					}
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
/*
 *
 */
uint32_t uiGetDinMask()
{
	uint32_t uiMask = 0;
	for (uint8_t i =0; i< DIN_CHANNEL;i++)
	{
		uiMask<<1;
		uiMask |= (xDinConfig[ i ].ucValue & 0x01 );
	}
	return ( uiMask );
}

#endif /* PDM_INPUT_C_ */
