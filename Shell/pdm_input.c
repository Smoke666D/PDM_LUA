/*
 * pdm_input.c
 *
 *  Created on: Jul 11, 2022
 *      Author: igor.dymov
 */

#ifndef PDM_INPUT_C_
#define PDM_INPUT_C_

#include "pdm_input.h"



osMessageQueueId_t CaptureTIMHandle;
uint8_t CaptureTIMBuffer[ 16 * sizeof( CAPTURE_DATA_TYPE ) ];
StaticQueue_t CaptureTIMControlBlock;
const osMessageQueueAttr_t CaptureTIM_attributes = {
	  .name = "CaptureTIM",
	  .cb_mem = &CaptureTIMControlBlock,
	  .cb_size = sizeof(CaptureTIMControlBlock),
	  .mq_mem = &CaptureTIMBuffer,
	  .mq_size = sizeof(CaptureTIMBuffer)
};


DoutCinfig sDinConfig[DIN_CHANNEL]={{{Din1_Pin,GPIOE},DIGITAL,0,0,0},{{Din2_Pin,GPIOE},DIGITAL,0,0,0},{{Din3_Pin,GPIOE},DIGITAL,0,0,0},{{Din4_Pin,GPIOE},DIGITAL,0,0,0},
								    {{Din5_Pin,GPIOE},DIGITAL,0,0,0},{{Din6_Pin,GPIOE},DIGITAL,0,0,0},{{Din7_Pin,GPIOE},DIGITAL,0,0,0},{{Din8_Pin,GPIOB},DIGITAL,0,0,0},
								    {{Din9_Pin,GPIOB},DIGITAL,0,0,0},{{Din10_Pin,GPIOF},DIGITAL,0,0,0},{{Din11_Pin,GPIOF},DIGITAL,0,0,0},{{Din12_Pin,GPIOF},DIGITAL,0,0,0} };

void SetCaptureData( PDM_INPUT_NAME channel, uint32_t data)
{
	static portBASE_TYPE xHigherPriorityTaskWoken;
	xHigherPriorityTaskWoken = pdFALSE;
	CAPTURE_DATA_TYPE temp;
	temp.channel = channel;
	temp.data = data;
	xQueueSendFromISR(CaptureTIMHandle, data, &xHigherPriorityTaskWoken );
	portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
	return;

}

PDM_INPUT_CONFIG_ERROR inputConfig( PDM_INPUT_NAME channel, PDM_INPUT_MODE mode)
{
	PDM_INPUT_CONFIG_ERROR res;
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	switch (mode)
	{
		case  DIGITAL:
			GPIO_InitStruct.Pin = sDinConfig[channel-1].pin_data.Pin;
			GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
			GPIO_InitStruct.Pull = GPIO_PULLDOWN;
			HAL_GPIO_Init(sDinConfig[channel-1].pin_data.GPIOx,&GPIO_InitStruct);
			sDinConfig[channel-1].mode = mode;
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
	return res;

}



void vDinTask(void *argument)
{

	uint8_t size = 0;
	CAPTURE_DATA_TYPE capture_data;
	inputConfig(INPUT1,DIGITAL);
	inputConfig(INPUT2,DIGITAL);
	inputConfig(INPUT3,DIGITAL);
	inputConfig(INPUT4,DIGITAL);
	inputConfig(INPUT5,DIGITAL);
	inputConfig(INPUT6,DIGITAL);
	inputConfig(INPUT7,DIGITAL);
	inputConfig(INPUT8,DIGITAL);
	inputConfig(INPUT9,DIGITAL);
	inputConfig(INPUT10,DIGITAL);
	inputConfig(INPUT11,DIGITAL);
	inputConfig(INPUT12,DIGITAL);
	CaptureTIMHandle = osMessageQueueNew (16, sizeof(CAPTURE_DATA_TYPE), &CaptureTIM_attributes);

	while(1)
	{
		vTaskDelay(1);
		size = uxQueueMessagesWaiting( CaptureTIMHandle);
		if (size!=0)
		{
		   for (int i=0;i<size;i++)
		   {
				xQueueReceive(CaptureTIMHandle, &capture_data, 0U );
				sDinConfig[capture_data.channel].data = capture_data.data;
		    }
		}
		for (int i = 0; i<DIN_CHANNEL; i++)
		{
			if ( sDinConfig[i].mode == DIGITAL )
			{
				if ( sDinConfig[i].data != HAL_GPIO_ReadPin(sDinConfig[i].pin_data.GPIOx,sDinConfig[i].pin_data.Pin) )
				{
					sDinConfig[i].counter++;
					if (sDinConfig[i].counter > DIN_VALID )
					{
						sDinConfig[i].counter = 0;
						sDinConfig[i].data = HAL_GPIO_ReadPin(sDinConfig[i].pin_data.GPIOx,sDinConfig[i].pin_data.Pin);
					}
				}

			}
		}
	}
}



#endif /* PDM_INPUT_C_ */
