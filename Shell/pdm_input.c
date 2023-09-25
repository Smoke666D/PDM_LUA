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
const  PIN_CONFIG xDinPortConfig[DIN_CHANNEL]= {{Din1_Pin,Din1_GPIO_Port},
											    {Din2_Pin,Din2_GPIO_Port},
											    {Din3_Pin,Din3_GPIO_Port},
											    {Din4_Pin,Din4_GPIO_Port},
											    {Din5_Pin,Din5_GPIO_Port},
											    {Din6_Pin,Din6_GPIO_Port},
											    {Din7_Pin,Din7_GPIO_Port},
											    {Din8_Pin,Din8_GPIO_Port},
											    {Din9_Pin,Din9_GPIO_Port},
											    {Din10_Pin,Din10_GPIO_Port},
											    {Din11_Pin,Din11_GPIO_Port}
#ifdef PCM
											    ,{Din12_Pin,Din12_GPIO_Port},
											    {Din13_Pin,Din13_GPIO_Port},
											    {Din14_Pin,Din14_GPIO_Port},
											    {Din15_Pin,Din15_GPIO_Port},
											    {Din16_Pin,Din16_GPIO_Port},
											    {Din17_Pin,Din17_GPIO_Port},
											    {Din18_Pin,Din18_GPIO_Port},
											    {Din19_Pin,Din19_GPIO_Port},
											    {Din20_Pin,Din20_GPIO_Port}
#endif
};
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

#define A 240


 uint16_t vRCFilter1( uint16_t input,uint16_t * old_output)
{

	volatile uint32_t new = input;
	volatile uint32_t old = *old_output;


	volatile uint16_t  output =  ( A * old + (256-A)*new )>>8;
	//*old_input = input;
	*old_output = output;
	return output;
}
uint16_t old1 = 0;
uint16_t old2 = 0;
/*
 *
 */
uint16_t uGetRPM1()
{
	uint16_t usTemp = 0U;
	if ((eRPM[0].uiData != 0) && (xDinConfig[INPUT_9].eInputType == RPM_CONFIG))
	{

		usTemp =/*(uint16_t)(( 1.0/( (float)*/eRPM[0].uiData /** 0.00005D) )* 60)*/;
		usTemp = vRCFilter1(usTemp,&old1);
	}
	return (  usTemp  );
}
/*
 *
 */

uint16_t uGetRPM2()
{
	uint16_t usTemp = 0U;
	if ( (eRPM[ 1 ].uiData != 0) && ( xDinConfig[INPUT_6].eInputType == RPM_CONFIG ) )
	{
		usTemp =(uint16_t)(( 1.0/( (float)eRPM[1].uiData * 0.00005D) )* 60);
		usTemp = vRCFilter1(usTemp,&old1);
	}

	return ( usTemp );
}
/*
 *
 */
static void vCheckRPM( uint8_t index)
{
	eRPM[index].usValidCounter++;
	if (eRPM[index].usValidCounter ==100)
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
	if (TimInd <= 2U)
	{
		eRPM[TimInd].uiRawData[eRPM[TimInd].ucCounter] = HAL_TIM_ReadCapturedValue( (TimInd == 0) ? &htim10 : &htim9 , TIM_CHANNEL_1 );
	   __HAL_TIM_SET_COUNTER( (TimInd==0) ? &htim10 : &htim9, 0x0000);
	   eRPM[TimInd].ucCounter ++;
	   eRPM[TimInd].ucValid = 1;
	   if (eRPM[TimInd].ucCounter == CC_MAX)
	   {
		   unsigned long ulTemp = 0;
		   eRPM[TimInd].ucCounter = 0;
		   for (uint8_t i=0; i < CC_MAX; i++)
		   {
			   ulTemp += eRPM[TimInd].uiRawData[i];
		   }
		   eRPM[TimInd].uiData = ( ulTemp / CC_MAX )* 2U;
	   	}
	}
	return;
}
/*
 *
 */
PDM_INPUT_CONFIG_ERROR eDinConfig( uint8_t ucCh, PDM_INPUT_TYPE inType, uint32_t ulHFront, uint32_t ulLFront)
{
	PDM_INPUT_CONFIG_ERROR eRes = WRONG_CHANNEL_NUMBER;
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	if ( ucCh < DIN_CHANNEL)
	{
		xDinConfig[ucCh].eInputType = inType;
		xDinConfig[ucCh].ucValue 	= (xDinConfig[ucCh].eInputType == DIN_CONFIG_POSITIVE ) ? 0U : 1U;
		GPIO_InitStruct.Pin 		= xDinPortConfig[ucCh].Pin;
		if ( ( ( ucCh == INPUT_9 ) || ( ucCh == INPUT_6 ) ) && ( inType == RPM_CONFIG ))
		{
			uint8_t ucRPMCh 				= (ucCh == INPUT_9) ? 0 : 1;
			eRPM[ucRPMCh ].ucCounter 		= 0U;
			eRPM[ucRPMCh ].uiData 			= 0U;
			eRPM[ucRPMCh ].usValidCounter 	= 0U;
			eRPM[ucRPMCh ].ucValid 			= 0U;
			GPIO_InitStruct.Mode 			= GPIO_MODE_AF_PP;
			GPIO_InitStruct.Pull 			= GPIO_NOPULL;
			GPIO_InitStruct.Speed 			= GPIO_SPEED_FREQ_LOW;
			GPIO_InitStruct.Alternate 		= (ucCh == INPUT_9) ? GPIO_AF3_TIM10 : GPIO_AF3_TIM9;
			HAL_GPIO_Init(xDinPortConfig[ucCh].GPIOx, &GPIO_InitStruct);
			if (ucCh == INPUT_9)
			{
				__HAL_RCC_TIM10_CLK_ENABLE();
			}
			else
			{
				__HAL_RCC_TIM9_CLK_ENABLE();
			}
			HAL_NVIC_SetPriority((ucCh == INPUT_9) ? TIM1_UP_TIM10_IRQn : TIM1_BRK_TIM9_IRQn , 5, 0);
			HAL_NVIC_EnableIRQ( (ucCh == INPUT_9) ? TIM1_UP_TIM10_IRQn : TIM1_BRK_TIM9_IRQn);
			HAL_TIM_IC_Start_IT((ucCh == INPUT_9) ? &htim10 : &htim9, TIM_CHANNEL_1 );
		}
		else
		{
			if ( xDinConfig[ucCh].eInputType == RPM_CONFIG )
			{
				xDinConfig[ucCh].eInputType = DIN_CONFIG_POSITIVE;
			}
			GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
			GPIO_InitStruct.Pull = GPIO_PULLUP;
			HAL_GPIO_Init(xDinPortConfig[ucCh].GPIOx,&GPIO_InitStruct);
			xDinConfig[ucCh].ulHighCounter = ulHFront;
			xDinConfig[ucCh].ulLowCounter = ulLFront;
			xDinConfig[ucCh].ucTempValue = 0U;
		}
		eRes = CONFIG_OK;
	}
	return ( eRes );
}
/*
 *
 */
void vDinInit( void )
{
	eDinConfig( INPUT_1, DIN_CONFIG_POSITIVE , DEF_H_FRONT, DEF_L_FRONT );
	eDinConfig( INPUT_2, DIN_CONFIG_POSITIVE , DEF_H_FRONT, DEF_L_FRONT );
	eDinConfig( INPUT_3, DIN_CONFIG_POSITIVE , DEF_H_FRONT, DEF_L_FRONT );
	eDinConfig( INPUT_4, DIN_CONFIG_POSITIVE , DEF_H_FRONT, DEF_L_FRONT );
	eDinConfig( INPUT_5, DIN_CONFIG_POSITIVE , DEF_H_FRONT, DEF_L_FRONT );
	eDinConfig( INPUT_6, DIN_CONFIG_POSITIVE , DEF_H_FRONT, DEF_L_FRONT );
	eDinConfig( INPUT_7, DIN_CONFIG_POSITIVE , DEF_H_FRONT, DEF_L_FRONT );
	eDinConfig( INPUT_8, DIN_CONFIG_POSITIVE , DEF_H_FRONT, DEF_L_FRONT );
	eDinConfig( INPUT_9, DIN_CONFIG_POSITIVE , DEF_H_FRONT, DEF_L_FRONT );
	eDinConfig( INPUT_10, DIN_CONFIG_POSITIVE ,DEF_H_FRONT, DEF_L_FRONT );
	eDinConfig( INPUT_11, DIN_CONFIG_POSITIVE ,DEF_H_FRONT, DEF_L_FRONT );
#ifdef PCM
	eDinConfig( INPUT_12, DIN_CONFIG_POSITIVE , DEF_H_FRONT, DEF_L_FRONT );
	eDinConfig( INPUT_13, DIN_CONFIG_POSITIVE , DEF_H_FRONT, DEF_L_FRONT );
	eDinConfig( INPUT_14, DIN_CONFIG_POSITIVE , DEF_H_FRONT, DEF_L_FRONT );
	eDinConfig( INPUT_15, DIN_CONFIG_POSITIVE , DEF_H_FRONT, DEF_L_FRONT );
	eDinConfig( INPUT_16, DIN_CONFIG_POSITIVE , DEF_H_FRONT, DEF_L_FRONT );
	eDinConfig( INPUT_17, DIN_CONFIG_POSITIVE , DEF_H_FRONT, DEF_L_FRONT );
	eDinConfig( INPUT_18, DIN_CONFIG_POSITIVE , DEF_H_FRONT, DEF_L_FRONT );
	eDinConfig( INPUT_19, DIN_CONFIG_POSITIVE , DEF_H_FRONT, DEF_L_FRONT );
	eDinConfig( INPUT_20, DIN_CONFIG_POSITIVE ,DEF_H_FRONT, DEF_L_FRONT );
#endif
	return;
}
/*
 *
 */
void vDinTask(void *argument)
{
	pxPDMstatusEvent = osLUAetPDMstatusHandle();
	while(1)
	{
		vTaskDelay(1);
		xEventGroupWaitBits(* pxPDMstatusEvent, RUN_STATE, pdFALSE, pdTRUE, portMAX_DELAY );
		ucTicTime = usDinGetTimer();
		for (uint8_t i = 0U; i < DIN_CHANNEL; i++)
		{
				if ( xDinConfig[i].eInputType != RPM_CONFIG )
				{
					uint8_t uсDinState = HAL_GPIO_ReadPin( xDinPortConfig[i].GPIOx, xDinPortConfig[i].Pin);
					if (uсDinState != xDinConfig[i].ucTempValue )
					{
						xDinConfig[i].ulCounter +=  ucTicTime ;
						if (xDinConfig[i].ulCounter > ( (xDinConfig[i].ucTempValue == GPIO_PIN_RESET) ? xDinConfig[i].ulHighCounter : xDinConfig[i].ulLowCounter ) )
						{

							xDinConfig[i].ucValue = uсDinState  ^ ( (~xDinConfig[i].eInputType) & 0x1);
							xDinConfig[i].ucTempValue = uсDinState ;
						}
					}
					else
					{
						xDinConfig[i].ulCounter = 0U;
					}
				}
				else
				{
					vCheckRPM(( i == INPUT_9 )? 0U : 1U );
				}
		}
	}
	return;
}
/*
 *
*/
uint8_t ucDinGet( DIN_INPUT_NAME eChNum )
{
	return ( (eChNum < DIN_CHANNEL) ? xDinConfig[ eChNum ].ucValue: 0U );
}
/*
 *
 */
uint32_t uiGetDinMask()
{
	uint32_t uiMask = 0;
	for (int8_t i = (DIN_CHANNEL -1);  i > -1 ; i--)
	{
		uiMask <<=1;
		uiMask |= ( xDinConfig[ i ].ucValue & 0x01 );
	}
	return ( uiMask );
}

#endif /* PDM_INPUT_C_ */
