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
static uint8_t  cc_counter						__SECTION(RAM_SECTION_CCMRAM) = 0;
static uint8_t  cc_counter1						__SECTION(RAM_SECTION_CCMRAM) = 0;
static uint32_t cc_data							__SECTION(RAM_SECTION_CCMRAM);
static uint32_t cc_data1					    __SECTION(RAM_SECTION_CCMRAM);
static uint32_t CC_Data[3]						__SECTION(RAM_SECTION_CCMRAM);
static uint32_t CC_Data1[3]						__SECTION(RAM_SECTION_CCMRAM);
static DinConfig_t xDinConfig[DIN_CHANNEL]  	__SECTION(RAM_SECTION_CCMRAM);
static EventGroupHandle_t  * pxPDMstatusEvent	__SECTION(RAM_SECTION_CCMRAM) = NULL;
#define CC_MAX  3
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

uint16_t temp111 = 0;
uint16_t uGetRPM1()
{
	temp111 = (xDinConfig[INPUT_9].eInputType==RPM_CONFIG) ? (uint16_t)(( 1.0/( (float)cc_data * 0.0002) )* 60) : 0U;
	return temp111;

			;
}
uint16_t uGetRPM2()
{
	return (
			(xDinConfig[INPUT_6].eInputType==RPM_CONFIG)
			? (uint16_t)( 1.0/( (float)cc_data * 0.0001) )* 60
			: 0U
	);
}


/*
 *
 */
void vGetCCData(TIM_HandleTypeDef *htim, uint32_t Channel)
{
	CC_Data[cc_counter] = HAL_TIM_ReadCapturedValue(htim,Channel);
	__HAL_TIM_SET_COUNTER(htim, 0x0000);
	cc_counter++;
	if (cc_counter == CC_MAX)
	{
		unsigned long ulTemp = 0;
		cc_counter = 0;
		for (uint8_t i=0; i < CC_MAX; i++)
		{
			ulTemp = ulTemp + CC_Data[i];
		}
		cc_data = ulTemp/CC_MAX;
	}
}
/*
 *
 */
void vGetCCData1(TIM_HandleTypeDef *htim, uint32_t Channel)
{
	CC_Data1[cc_counter1] = HAL_TIM_ReadCapturedValue(&htim,Channel);
	__HAL_TIM_SET_COUNTER(htim, 0x0000);
	cc_counter1++;
	if (cc_counter1 == CC_MAX)
	{
		unsigned long ulTemp = 0;
		cc_counter1 = 0;
		for (uint8_t i=0; i < CC_MAX; i++)
		{
			ulTemp = ulTemp + CC_Data1[i];
		}
		cc_data1 = ulTemp/CC_MAX;
	}
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
   				    __HAL_RCC_TIM9_CLK_ENABLE();
					__HAL_RCC_GPIOE_CLK_ENABLE();
					/**TIM9 GPIO Configuration
					PE5     ------> TIM9_CH1
					*/
					GPIO_InitStruct.Pin = Din6_Pin;
					GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
					GPIO_InitStruct.Pull = GPIO_NOPULL;
					GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
					GPIO_InitStruct.Alternate = GPIO_AF3_TIM9;
					HAL_GPIO_Init(Din6_GPIO_Port, &GPIO_InitStruct);
					/* TIM9 interrupt Init */
					HAL_NVIC_SetPriority(TIM1_BRK_TIM9_IRQn, 5, 0);
					HAL_NVIC_EnableIRQ(TIM1_BRK_TIM9_IRQn);
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
	cc_counter	= 0;
	cc_data = 0;
	cc_counter1	= 0;
	cc_data1 = 0;
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
