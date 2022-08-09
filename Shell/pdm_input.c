/*
 * pdm_input.c
 *
 *  Created on: Jul 11, 2022
 *      Author: igor.dymov
 */

#ifndef PDM_INPUT_C_
#define PDM_INPUT_C_

#include "pdm_input.h"
#include "FreeRTOS.h"
#include "system.h"
#include "task.h"
#include "semphr.h"
#include "event_groups.h"
#include "stm32f4xx_hal.h"
#include "cmsis_os.h"
#include "event_groups.h"
#include "task.h"
#include "semphr.h"


static uint16_t system_timer = 0;
static uint16_t timer = 0;
static uint8_t overload =0;

const  PIN_CONFIG DINPortConfig[DIN_CHANNEL]= {{Din1_Pin,GPIOE},
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


volatile DoutCinfig sDinConfig[DIN_CHANNEL]  __SECTION(RAM_SECTION_CCMRAM);

void SystemDinTimer(void)
{
	system_timer ++;
	if (system_timer  >= 65000)
	{
	 system_timer = 0;
	 overload = 1;
	}
}

uint16_t GetDinTimer(void)
{
 uint16_t delay =0;
 if (overload)
 {
	 overload = 0;
	 delay = 65000 - timer + system_timer;
 }
 else
 {
	 delay = system_timer-timer;
 }
 timer = system_timer;
 return delay;
}



PDM_INPUT_CONFIG_ERROR inputConfig( uint8_t channel, LOGIC_STATE ls)
{
	PDM_INPUT_CONFIG_ERROR res = WRONG_CHANNEL_NUMBER;
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	if (channel < DIN_CHANNEL)
	{
			GPIO_InitStruct.Pin = DINPortConfig[channel].Pin;
			GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
			GPIO_InitStruct.Pull = GPIO_PULLDOWN;
			HAL_GPIO_Init(DINPortConfig[channel].GPIOx,&GPIO_InitStruct);
			sDinConfig[channel].high_counter = 10;
			sDinConfig[channel].low_counter = 10;
			sDinConfig[channel].data = 0;
			sDinConfig[channel].temp_data = 0;
			sDinConfig[channel].state = ls;
			res = CONFIG_OK;
	}
	return res;

}


void vDinTask(void *argument)
{
	uint8_t delay = 0;
	inputConfig(INPUT1,POSITIVE_STATE );
	inputConfig(INPUT2,POSITIVE_STATE );
	inputConfig(INPUT3,POSITIVE_STATE );
	inputConfig(INPUT4,POSITIVE_STATE );
	inputConfig(INPUT5,POSITIVE_STATE );
	inputConfig(INPUT6,POSITIVE_STATE );
	inputConfig(INPUT7,POSITIVE_STATE );
	inputConfig(INPUT8,POSITIVE_STATE );
	inputConfig(INPUT9,POSITIVE_STATE );
	inputConfig(INPUT10,POSITIVE_STATE );
	inputConfig(INPUT11,POSITIVE_STATE );
	while(1)
	{
		vTaskDelay(1);
		delay= GetDinTimer();
		for (uint8_t i = 0; i<DIN_CHANNEL; i++)
		{
				sDinConfig[i].counter +=  delay ;
				switch (HAL_GPIO_ReadPin(DINPortConfig[i].GPIOx,DINPortConfig[i].Pin))
				{
				    case GPIO_PIN_SET:
				    	if (sDinConfig[i].temp_data == GPIO_PIN_RESET)
				    	{
				    			if  (sDinConfig[i].counter >sDinConfig[i].high_counter)
				    			{
				    				sDinConfig[i].counter = 0;
				    				sDinConfig[i].data = (sDinConfig[i].state == NEGATIVE_STATE) ?0U:1U;
				    				sDinConfig[i].temp_data = GPIO_PIN_SET;
				    			}
				    	}
						else
						{
							sDinConfig[i].counter = 0;
						}
				    	break;
				    case GPIO_PIN_RESET:

				    	if (sDinConfig[i].temp_data == GPIO_PIN_SET)
				    	{
				    		if (sDinConfig[i].counter > sDinConfig[i].low_counter)
				    	    {
				    			sDinConfig[i].counter = 0;
				    			sDinConfig[i].data = (sDinConfig[i].state == NEGATIVE_STATE) ?1U:0U;
				    			sDinConfig[i].temp_data = GPIO_PIN_RESET;
				    	    }

				    	}
				    	else
				    	{
				    		sDinConfig[i].counter = 0;
				    	}
				    	break;
				    default:
				    	break;

				}
		}
	}
}

/*
 *
 */

uint8_t uDinGet(PDM_INPUT_NAME channel)
{

	return (channel < DIN_CHANNEL) ? sDinConfig[channel].data: 0U;
}





#endif /* PDM_INPUT_C_ */
