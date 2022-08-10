/*
 * cantask.c
 *
 *  Created on: Jul 5, 2022
 *      Author: igor.dymov
 */

#include "cantask.h"
#include "semphr.h"
#include "system.h"
#include "event_groups.h"
#include "luatask.h"
#include "CO_driver_ST32F4xx.h"


extern CAN_HandleTypeDef hcan1;
QueueHandle_t CanRXHandle;
QueueHandle_t CanTXHandle;




QueueHandle_t* pCANRXgetQueue ( void )
{
  return &CanRXHandle;
}

QueueHandle_t* pCANTXgetQueue ( void )
{
  return &CanTXHandle;
}

CANRX MailBoxBuffer[MAILBOXSIZE] __SECTION(RAM_SECTION_CCMRAM);
static   EventGroupHandle_t  * xPDMstatusEvent __SECTION(RAM_SECTION_CCMRAM) = NULL;


void setFilter(uint16_t mailboxindex)
{
	  CAN_FilterTypeDef  sFilterConfig;
	  uint16_t index = mailboxindex / 4;
	 sFilterConfig.FilterActivation = CAN_FILTER_ENABLE;
     sFilterConfig.FilterBank = index;
     sFilterConfig.FilterFIFOAssignment =0;
     sFilterConfig.FilterIdHigh =  MailBoxBuffer[index*4 +2 ].ident<<5 ;
     sFilterConfig.FilterIdLow  =MailBoxBuffer[index*4  ].ident<<5 ;
     sFilterConfig.FilterMaskIdHigh = MailBoxBuffer[index*4 +3 ].ident<<5 ;
     sFilterConfig.FilterMaskIdLow  =  MailBoxBuffer[index*4 +1 ].ident<<5 ;
     sFilterConfig.FilterMode = CAN_FILTERMODE_IDLIST;
     sFilterConfig.FilterScale =CAN_FILTERSCALE_16BIT;


     if (HAL_CAN_ConfigFilter(&hcan1, &sFilterConfig) != HAL_OK)
     {
         Error_Handler();
     }

}


void vCanInsertToRXQueue(CAN_FRAME_TYPE * data)
{
	static portBASE_TYPE xHigherPriorityTaskWoken;
	xHigherPriorityTaskWoken = pdFALSE;
	xQueueSendFromISR( CanRXHandle, data, &xHigherPriorityTaskWoken );
	portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
	return;
}


void InitMailBoxBuffer()
{
	for (int i=0;i<MAILBOXSIZE;i++)
	{
		 MailBoxBuffer[i].ident = 0U;
		 MailBoxBuffer[i].new_data = 0U;
	}
}

void SetWaitFilter(uint32_t id)
{
	 MailBoxBuffer[0].ident = id;
	 setFilter(0);
}
uint8_t CheckAnswer( void )
{
	 return MailBoxBuffer[0].new_data;
}

void SetMailboxFilter(uint32_t id)
{
	for (int i=1;i<MAILBOXSIZE;i++)
	{
		if (MailBoxBuffer[i].ident == 0U)
		{
			 MailBoxBuffer[i].ident = id;
			 setFilter(i);
			 break;
		}
	}
}

void ResetMailboxFilter(uint32_t id)
{
	for (int i=1;i<MAILBOXSIZE;i++)
	{
		if (MailBoxBuffer[i].ident == 0U)
		{
			 MailBoxBuffer[i].ident = 0;
			 MailBoxBuffer[i].new_data = 0;
			 break;
		}
	}
}



void vCanInsertRXData(CAN_FRAME_TYPE * RXPacket)
{
	uint16_t id = RXPacket->filter_id;
	if (MailBoxBuffer[id].ident == RXPacket->ident)
	{
			MailBoxBuffer[id].DLC = RXPacket->DLC;
			for (int k =0;k <RXPacket->DLC;k++)
			{
				 MailBoxBuffer[id].data[k] =  RXPacket->data[k];
			}
			MailBoxBuffer[id].new_data = 1;
			return;
	}
	return;
}

uint8_t vCanChekMessage(uint32_t id)
{
	uint8_t res = 0;
	uint8_t max_data =MAILBOXSIZE;
	for (int k=0;k < max_data;k++)
	{
		if ((MailBoxBuffer[k].new_data == 1) && (MailBoxBuffer[k].ident = id))
		{
				res = 1;
				break;
		}
	}
	return res;
}

uint8_t vCanGetRequest(CAN_FRAME_TYPE * RXPacket)
{
	uint8_t res =0;
	res = CheckAnswer();
	if (res!=0)
	{
		RXPacket->ident = MailBoxBuffer[0].ident;
		RXPacket->DLC = MailBoxBuffer[0].DLC;
		for (int i =0; i < RXPacket->DLC;i++)
		{
			RXPacket->data[i] = MailBoxBuffer[0].data[i];
		}
		MailBoxBuffer[0].new_data = 0;
		MailBoxBuffer[0].ident  =0;
	}
	return res;
}


uint8_t vCanGetMessage(CAN_FRAME_TYPE * RXPacket)
{
	uint8_t res = 0;
	for (int k=0;k < MAILBOXSIZE;k++)
	{
		if ((MailBoxBuffer[k].new_data == 1) && (MailBoxBuffer[k].ident == RXPacket->ident))
		{
			RXPacket->DLC = MailBoxBuffer[k].DLC;
			for (int i =0; i < RXPacket->DLC;i++)
			{
				RXPacket->data[i] = MailBoxBuffer[k].data[i];
			}
			MailBoxBuffer[k].new_data = 0;
			res = 1;
			break;
		}
	}
	return res;
}



void vCanInsertTXData(uint32_t CanID, uint8_t * data, uint8_t data_len )
{
	CAN_TX_FRAME_TYPE data_to_send;
	data_to_send.ident = CanID;
	data_to_send.DLC = data_len;
	for (uint8_t i=0; i<data_len;i++)
	{
		data_to_send.data[i] = data[i];
	}
	 xQueueSend(CanTXHandle,&data_to_send, portMAX_DELAY);

}


void vCANinit()
{
	InitMailBoxBuffer();
	vConfigCAN(&hcan1);
	CO_CANsetConfigurationMode();
	CO_CANmodule_disable();
	CO_CANmodule_init(1000);
	CO_CANsetNormalMode();
}
/*
 * Процесс для обработки can сообщений
 */

void vCanTXTask(void *argument)
{
	CAN_TX_FRAME_TYPE TXPacket;
	while(1)
	{
		xQueueReceive( CanTXHandle, &TXPacket, portMAX_DELAY);
		uPDMCanSend(&TXPacket);

	}
}


void vCanRXTask(void *argument)
{
	CAN_FRAME_TYPE RXPacket;
	while(1)
	{
		xQueueReceive( CanRXHandle, &RXPacket,  portMAX_DELAY );
		vCanInsertRXData(&RXPacket);
	}

}

