/*
 * cantask.c
 *
 *  Created on: Jul 5, 2022
 *      Author: igor.dymov
 */


#include "cmsis_os.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "event_groups.h"
#include "cantask.h"
#include "CO_driver_ST32F4xx.h"

#define RX_SIZE  10
#define TX_SIZE  10

extern CAN_HandleTypeDef hcan1;
extern osMessageQueueId_t CanRXHandle;
extern osMessageQueueId_t CanTXHandle;
CO_CANmodule_t   CO_PDM;
CO_CANrx_t  RX_BUFFER[RX_SIZE];
CO_CANtx_t  TX_BUFFER[TX_SIZE];

#define MAILBOXSIZE  50
CANtx MailBoxBuffer[MAILBOXSIZE];


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
	for (int i=0;i<MAILBOXSIZE;i++)
	{
		if (MailBoxBuffer[i].ident == RXPacket->ident)
		{
				MailBoxBuffer[i].DLC = RXPacket->DLC;
				for (int k =0;k <RXPacket->DLC;k++)
				{
					 MailBoxBuffer[i].data[k] =  RXPacket->data[k];
				}
				MailBoxBuffer[i].new_data = 1;
				return;
		}
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
	}
	return res;
}


uint8_t vCanGetMessage(CAN_FRAME_TYPE * RXPacket)
{
	uint8_t res = 0;
	for (int k=0;k < MAILBOXSIZE;k++)
	{
		if ((MailBoxBuffer[k].new_data == 1) && (MailBoxBuffer[k].ident = RXPacket->ident))
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
	CO_CANtx_t data_to_send;
	data_to_send.ident = CanID;
	data_to_send.DLC = data_len;
	data_to_send.bufferFull = false;
	data_to_send.syncFlag = false;
	for (uint8_t i=0; i<data_len;i++)
	{
		data_to_send.data[i] = data[i];
	}
	 xQueueSend(CanTXHandle,&data_to_send, portMAX_DELAY);

}

/*
 * Процесс для обработки can сообщений
 */
void vCanTask(void *argument)
{
	uint8_t size;
    CO_PDM.CANptr = &hcan1;
	CO_PDM.CANnormal = false;

	//Инициализация модуля CAN
	CO_CANtx_t TXPacket;
	CAN_FRAME_TYPE RXPacket;
	CO_CANsetConfigurationMode(CO_PDM.CANptr);
	CO_CANmodule_disable(&CO_PDM);
	CO_CANmodule_init(&CO_PDM,&hcan1,RX_BUFFER,RX_SIZE,TX_BUFFER,TX_SIZE,1000);
	CO_CANsetNormalMode(&CO_PDM);
	while(1)
	{
         //Проверяем исходящую очередь
		 size = uxQueueMessagesWaiting( CanTXHandle);
		 if (size!=0)
		 {
			 for (int i=0;i<size;i++)
			 {
				 xQueueReceive( CanTXHandle, &TXPacket, 0U );
				 CO_CANsend(&CO_PDM,&TXPacket);
			 }
		 }
		 //Проверяем входящую очередь
		 size = uxQueueMessagesWaiting( CanRXHandle);
		 if (size!=0)
		 {
			  for (int i=0;i<size;i++)
			  {
					xQueueReceive( CanRXHandle, &RXPacket, 0U );
					vCanInsertRXData(&RXPacket);
			  }
		 }
		 vTaskDelay(1);
	}

}

