/*
 * pdm_input.c
 *
 *  Created on: Jul 11, 2022
 *      Author: igor.dymov
 */

#ifndef PDM_INPUT_C_
#define PDM_INPUT_C_

#include "pdm_input.h"

#include "task.h"
#include "semphr.h"
#include "stm32f4xx_hal.h"
#include "task.h"
#include "semphr.h"


static uint16_t system_timer		__SECTION(RAM_SECTION_CCMRAM) = 0;
static uint16_t timer 				__SECTION(RAM_SECTION_CCMRAM) = 0;
static uint8_t overload				__SECTION(RAM_SECTION_CCMRAM) = 0;
static uint8_t delay   				__SECTION(RAM_SECTION_CCMRAM) = 0;
DoutCinfig sDinConfig[DIN_CHANNEL]  __SECTION(RAM_SECTION_CCMRAM);


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

static   EventGroupHandle_t  * xPDMstatusEvent __SECTION(RAM_SECTION_CCMRAM) = NULL;


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



PDM_INPUT_CONFIG_ERROR inputConfig( uint8_t channel, LOGIC_STATE ls, uint32_t hfront, uint32_t lfront)
{
	PDM_INPUT_CONFIG_ERROR res = WRONG_CHANNEL_NUMBER;
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	if (channel < DIN_CHANNEL)
	{
			GPIO_InitStruct.Pin = DINPortConfig[channel].Pin;
			GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
			GPIO_InitStruct.Pull = GPIO_PULLDOWN;
			HAL_GPIO_Init(DINPortConfig[channel].GPIOx,&GPIO_InitStruct);
			sDinConfig[channel].high_counter = hfront;
			sDinConfig[channel].low_counter = lfront;
			sDinConfig[channel].data = (ls == NEGATIVE_STATE) ?1U : 0U;
			sDinConfig[channel].temp_data = 0;
			sDinConfig[channel].state = ls;
			res = CONFIG_OK;
	}
	return res;

}

void vDINconfig( void )
{
	inputConfig(INPUT1,POSITIVE_STATE,DEF_H_FRONT,DEF_L_FRONT );
	inputConfig(INPUT2,POSITIVE_STATE,DEF_H_FRONT,DEF_L_FRONT );
	inputConfig(INPUT3,POSITIVE_STATE,DEF_H_FRONT,DEF_L_FRONT );
	inputConfig(INPUT4,POSITIVE_STATE,DEF_H_FRONT,DEF_L_FRONT  );
	inputConfig(INPUT5,POSITIVE_STATE,DEF_H_FRONT,DEF_L_FRONT );
	inputConfig(INPUT6,POSITIVE_STATE,DEF_H_FRONT,DEF_L_FRONT  );
	inputConfig(INPUT7,POSITIVE_STATE,DEF_H_FRONT,DEF_L_FRONT );
	inputConfig(INPUT8,POSITIVE_STATE,DEF_H_FRONT,DEF_L_FRONT  );
	inputConfig(INPUT9,POSITIVE_STATE,DEF_H_FRONT,DEF_L_FRONT  );
	inputConfig(INPUT10,POSITIVE_STATE,DEF_H_FRONT,DEF_L_FRONT  );
	inputConfig(INPUT11,POSITIVE_STATE,DEF_H_FRONT,DEF_L_FRONT  );

}

void vDinTask(void *argument)
{
	uint32_t cur_din_state;
	xPDMstatusEvent = osLUAetPDMstatusHandle();
	while(1)
	{
		vTaskDelay(1);
		xEventGroupWaitBits(* xPDMstatusEvent, RUN_STATE, pdFALSE, pdTRUE, portMAX_DELAY );
		delay= GetDinTimer();
		for (uint8_t i = 0; i<DIN_CHANNEL; i++)
		{

				cur_din_state = HAL_GPIO_ReadPin(DINPortConfig[i].GPIOx,DINPortConfig[i].Pin);

				if (cur_din_state != sDinConfig[i].temp_data )
				{
					sDinConfig[i].counter +=  delay ;
					if (sDinConfig[i].counter > (sDinConfig[i].temp_data == GPIO_PIN_RESET) ? sDinConfig[i].high_counter : sDinConfig[i].low_counter )
					{
						if (cur_din_state == GPIO_PIN_SET)
							sDinConfig[i].data = (sDinConfig[i].state == NEGATIVE_STATE) ?0U:1U;
						else
							sDinConfig[i].data = (sDinConfig[i].state == NEGATIVE_STATE) ?1U:0U;
						sDinConfig[i].temp_data = cur_din_state ;
					}

				}
				else
				{
					sDinConfig[i].counter = 0;
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
