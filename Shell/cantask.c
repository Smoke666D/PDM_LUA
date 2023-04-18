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

#include "CO_driver_ST32F4xx.h"


extern CAN_HandleTypeDef hcan1;
static CAN_ERROR_TYPE eCanError __SECTION(RAM_SECTION_CCMRAM);

QueueHandle_t pCanRXHandle  __SECTION(RAM_SECTION_CCMRAM);
QueueHandle_t pCanTXHandle  __SECTION(RAM_SECTION_CCMRAM);
CANRX MailBoxBuffer[MAILBOXSIZE] __SECTION(RAM_SECTION_CCMRAM);


/*
 *
 */
QueueHandle_t* pCANRXgetQueue ( void )
{
  return ( &pCanRXHandle );
}
/*
 *
 */
QueueHandle_t* pCANTXgetQueue ( void )
{
  return ( &pCanTXHandle );
}
/*
 *
 */
void vFilterSet(uint16_t mailboxindex)
{
	 CAN_FilterTypeDef  sFilterConfig;
	 uint16_t index = mailboxindex / 4;
	 sFilterConfig.FilterActivation = CAN_FILTER_ENABLE;
     sFilterConfig.FilterBank = index;
     sFilterConfig.FilterFIFOAssignment =0;
     sFilterConfig.FilterIdHigh 	= (MailBoxBuffer[index*4U +2U ].ident) <<5U ;
     sFilterConfig.FilterIdLow  	= (MailBoxBuffer[index*4U  ].ident)    <<5U ;
     sFilterConfig.FilterMaskIdHigh = (MailBoxBuffer[index*4U +3U ].ident) <<5U ;
     sFilterConfig.FilterMaskIdLow  = (MailBoxBuffer[index*4U +1U ].ident) <<5U ;
     sFilterConfig.FilterMode = CAN_FILTERMODE_IDLIST;
     sFilterConfig.FilterScale =CAN_FILTERSCALE_16BIT;
     HAL_CAN_ConfigFilter(&hcan1, &sFilterConfig);
     return;
}

/*
 *
 */
void vCanInsertToRXQueue(CAN_FRAME_TYPE * data)
{
	static portBASE_TYPE xHigherPriorityTaskWoken;
	xHigherPriorityTaskWoken = pdFALSE;
	xQueueSendFromISR( pCanRXHandle, data, &xHigherPriorityTaskWoken );
	portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
	return;
}

/*
 *
 */
static void vInitMailBoxBuffer( void )
{
	for (int i=0;i<MAILBOXSIZE;i++)
	{
		 MailBoxBuffer[i].ident = 0U;
		 MailBoxBuffer[i].new_data = 0U;
	}
	return;
}
/*
 *
 */
void vSetWaitFilter(uint32_t id)
{
	 MailBoxBuffer[0].ident = id;
	 vFilterSet(0);
	 return;
}
/*
 *
 */
uint8_t vCheckAnswer( void )
{
	 return ( MailBoxBuffer[0].new_data );
}
/*
 *
 */
ERROR_TYPE_t eMailboxFilterSet(uint32_t id)
{
	ERROR_TYPE_t eRes = BUFFER_FULL;
	for (int i=1;i<MAILBOXSIZE;i++)
	{
		if (MailBoxBuffer[i].ident == 0U)
		{
			 MailBoxBuffer[i].ident = id;
			 vFilterSet(i);
			 eRes = ERROR_NO;
			 break;
		}
	}
	return ( eRes );
}
/*
 *
 */
void vResetMailboxFilter(uint32_t id)
{
	for (int i=1;i<MAILBOXSIZE;i++)
	{
		if (MailBoxBuffer[i].ident == id)
		{
			 MailBoxBuffer[i].ident = 0U;
			 MailBoxBuffer[i].new_data = 0U;
			 break;
		}
	}
	return;
}
/*
 *
 */
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
	uint8_t ucRes = 0;
	uint8_t max_data = MAILBOXSIZE;
	for (int k=0;k < max_data;k++)
	{
		if ((MailBoxBuffer[k].new_data == 1) && (MailBoxBuffer[k].ident = id))
		{
				ucRes = 1U;
				break;
		}
	}
	return ( ucRes );
}
/*
 *
 */
uint8_t vCanGetRequest(CAN_FRAME_TYPE * RXPacket)
{
	uint8_t res = 0U;
	res = vCheckAnswer();
	if ( res != 0U )
	{
		RXPacket->ident = MailBoxBuffer[0].ident;
		RXPacket->DLC = MailBoxBuffer[0].DLC;
		for (int i =0; i < RXPacket->DLC;i++)
		{
			RXPacket->data[i] = MailBoxBuffer[ 0U ].data[i];
		}
		MailBoxBuffer[0].new_data 	= 0U;
		MailBoxBuffer[0].ident 		= 0U;
	}
	return ( res );
}
/*
 *
 */
uint8_t vCanGetMessage(CAN_FRAME_TYPE * RXPacket)
{
	uint8_t res = 0U;
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
			res = 1U;
			break;
		}
	}
	return ( res );
}

/*
 *
 */
void vCanInsertTXData(uint32_t CanID, uint8_t * data, uint8_t data_len )
{
	CAN_TX_FRAME_TYPE data_to_send;
	data_to_send.ident = CanID;
	data_to_send.DLC   = data_len;
	for (uint8_t i=0; i<data_len;i++)
	{
		data_to_send.data[i] = data[i];
	}
	xQueueSend(pCanTXHandle, &data_to_send, 1U);
	return;
}

static uint16_t boundrate_can1;
/*
 *
 */
void vCANBoudInit( uint16_t boudrate )
{

	CO_CANsetConfigurationMode();
	CO_CANmodule_disable();
	boundrate_can1 = boudrate;
	CO_CANmodule_init( boundrate_can1);
	CO_CANsetNormalMode();
    return;
}

void vReinit()
{
	xQueueReset(pCanTXHandle);
	xQueueReset(pCanRXHandle);
	vCANBoudInit(boundrate_can1);

}
/*
 *
 */

void vCANinit()
{
	eCanError = CAN_OFF;
	vInitMailBoxBuffer();
	vConfigCAN(&hcan1);
	//vCANBoudInit( 1000 );
	return;
}
/*
 * Процесс для обработки can сообщений
 */
void vCanTXTask(void *argument)
{
	CAN_TX_FRAME_TYPE TXPacket;
	while(1)
	{

		xQueuePeek( pCanTXHandle, &TXPacket, portMAX_DELAY);
		if (uPDMGetCanReady() > 0 )
		{
			xQueueReceive( pCanTXHandle, &TXPacket, 1);
			uPDMCanSend(&TXPacket);
			eCanError = CAN_NORMAL;
		}
	}
}

void HAL_CAN_WakeUpFromRxMsgCallback(CAN_HandleTypeDef *hcan)
{

}

static uint32_t error;

void HAL_CAN_ErrorCallback(CAN_HandleTypeDef *hcan)
{


	if __HAL_CAN_GET_FLAG(hcan,CAN_FLAG_TERR0)
	{
		HAL_CAN_AbortTxRequest(hcan,CAN_TX_MAILBOX0);
	}
	if __HAL_CAN_GET_FLAG(hcan,CAN_FLAG_TERR1)
	{
		HAL_CAN_AbortTxRequest(hcan,CAN_TX_MAILBOX1);
	}
	if __HAL_CAN_GET_FLAG(hcan,CAN_FLAG_TERR2)
	{
		HAL_CAN_AbortTxRequest(hcan,CAN_TX_MAILBOX2);
	}
	error =hcan->ErrorCode;
	hcan->ErrorCode = HAL_CAN_ERROR_NONE;
	if (error & HAL_CAN_ERROR_ACK)
	{
		eCanError = CAN_CUT_OFF;
	}
	if  ((error & HAL_CAN_ERROR_BD) || (error & HAL_CAN_ERROR_BR ))
	{
		eCanError = CAN_SHORT_CUT;
	}


}

/*
 *
 */
void vCanRXTask(void *argument)
{
	CAN_FRAME_TYPE RXPacket;
	while(1)
	{
		xQueueReceive( pCanRXHandle, &RXPacket,  portMAX_DELAY );
		vCanInsertRXData(&RXPacket);
		eCanError = CAN_NORMAL;
	}
}
