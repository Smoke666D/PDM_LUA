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

volatile DoutCinfig sDinConfig[12]={{Din1_Pin,GPIOE,DIGITAL,0,0,10,10,0,POSITIVE_STATE},
									{Din2_Pin,GPIOE,DIGITAL,0,0,10,10,0,POSITIVE_STATE},
									{Din3_Pin,GPIOE,DIGITAL,0,0,10,10,0,POSITIVE_STATE},
									{Din4_Pin,GPIOE,DIGITAL,0,0,10,10,0,POSITIVE_STATE},
								    {Din5_Pin,GPIOE,DIGITAL,0,0,10,10,0,POSITIVE_STATE},
									{Din6_Pin,GPIOE,DIGITAL,0,0,10,10,0,POSITIVE_STATE},
									{Din7_Pin,GPIOE,DIGITAL,0,0,10,10,0,POSITIVE_STATE},
									{Din8_Pin,GPIOB,DIGITAL,0,0,10,10,0,POSITIVE_STATE},
								    {Din9_Pin,GPIOB,DIGITAL,0,0,10,10,0,POSITIVE_STATE},
									{Din10_Pin,GPIOF,DIGITAL,0,0,10,10,0,POSITIVE_STATE},
									{Din11_Pin,GPIOF,DIGITAL,0,0,10,10,0,POSITIVE_STATE}};

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



PDM_INPUT_CONFIG_ERROR inputConfig( uint8_t channel, PDM_INPUT_MODE mode,LOGIC_STATE ls)
{
	PDM_INPUT_CONFIG_ERROR res = WRONG_CHANNEL_NUMBER;
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	if (channel < DIN_CHANNEL)
	{
		switch (mode)
		{
			case  DIGITAL:
				GPIO_InitStruct.Pin = sDinConfig[channel].Pin;
				GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
				GPIO_InitStruct.Pull = GPIO_PULLDOWN;
				HAL_GPIO_Init(sDinConfig[channel].GPIOx,&GPIO_InitStruct);
				sDinConfig[channel].mode = mode;
				sDinConfig[channel].temp_data = 0;
				sDinConfig[channel].state = ls;
				res = CONFIG_OK;
				break;
			case CAPTURE:
				switch (channel)
				{
				case INPUT6:
				case INPUT7:
					break;
				default:
					res =CONFIG_NO_SUPPORT;
				}
				break;
				default:
					res =WRONG_CHANNEL_MODE;
					break;
		}
	}
	return res;

}



void vDinTask(void *argument)
{


	uint8_t delay = 0;

	inputConfig(INPUT1,DIGITAL,POSITIVE_STATE );
	inputConfig(INPUT2,DIGITAL,POSITIVE_STATE );
	inputConfig(INPUT3,DIGITAL,POSITIVE_STATE );
	inputConfig(INPUT4,DIGITAL,POSITIVE_STATE );
	inputConfig(INPUT5,DIGITAL,POSITIVE_STATE );
	inputConfig(INPUT6,DIGITAL,POSITIVE_STATE );
	inputConfig(INPUT7,DIGITAL,POSITIVE_STATE );
	inputConfig(INPUT8,DIGITAL,POSITIVE_STATE );
	inputConfig(INPUT9,DIGITAL,POSITIVE_STATE );
	inputConfig(INPUT10,DIGITAL,POSITIVE_STATE );
	inputConfig(INPUT11,DIGITAL,POSITIVE_STATE );

	while(1)
	{
		vTaskDelay(1);
		delay= GetDinTimer();
		for (uint8_t i = 0; i<DIN_CHANNEL; i++)
		{
			if ( sDinConfig[i].mode == DIGITAL )
			{
				sDinConfig[i].counter= sDinConfig[i].counter + delay ;
				switch (HAL_GPIO_ReadPin(sDinConfig[i].GPIOx,sDinConfig[i].Pin))
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
				    		if (sDinConfig[i].counter > sDinConfig[i].high_counter)
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
}

/*
 *
 */

uint8_t uDinGet(PDM_INPUT_NAME channel)
{
	uint8_t res = 0;
	if (channel<DIN_CHANNEL)
	{
		res = sDinConfig[channel].data;
	}
	return res;
}





#endif /* PDM_INPUT_C_ */
