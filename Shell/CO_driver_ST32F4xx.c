/*
 * CAN module object for generic microcontroller.
 *
 * This file is a template for other microcontrollers.
 *
 * @file        CO_driver.c
 * @ingroup     CO_driver
 * @author      Janez Paternoster
 * @copyright   2004 - 2020 Janez Paternoster
 *
 * This file is part of CANopenNode, an opensource CANopen Stack.
 * Project home page is <https://github.com/CANopenNode/CANopenNode>.
 * For more information on CANopen see <http://www.can-cia.org/>.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#include "CO_driver_ST32F4xx.h"
#include "cantask.h"


/* CAN masks for identifiers */
#define CANID_MASK                              0x07FF  /*!< CAN standard ID mask */
#define FLAG_RTR                                0x8000  /*!< RTR flag, part of identifier */


CAN_HandleTypeDef  * pPDMCan;


void vConfigCAN(CAN_HandleTypeDef  * pcan)
{
	pPDMCan = pcan;

}

/******************************************************************************/
void CO_CANsetConfigurationMode()
{
    /* Put CAN module in configuration mode */
	 if (pPDMCan != NULL) {
		 HAL_CAN_Stop(pPDMCan);
	 }
}

/******************************************************************************/
void CO_CANsetNormalMode(){
    /* Put CAN module in normal mode */
	if (pPDMCan != NULL)
	{
		HAL_CAN_Start(pPDMCan);
	}
}
/******************************************************************************/
CO_ReturnError_t CO_CANmodule_init(

        uint16_t                CANbitRate)
{
    //Конфигурация драйвера
    pPDMCan->Init.Mode = CAN_MODE_NORMAL;
    pPDMCan->Init.TimeTriggeredMode = DISABLE;
    pPDMCan->Init.AutoBusOff = DISABLE;
    pPDMCan->Init.AutoWakeUp = DISABLE;
    pPDMCan->Init.AutoRetransmission = ENABLE;
    pPDMCan->Init.ReceiveFifoLocked = DISABLE;
    pPDMCan->Init.TransmitFifoPriority = ENABLE;

    /* Configure CAN timing */
      switch (CANbitRate)
      {
            case 1000:  pPDMCan->Init.Prescaler = 2;
                   break;
            case 500:  pPDMCan->Init.Prescaler = 4;
                   break;
            case 250:  pPDMCan->Init.Prescaler = 8;
                   break;
            default:
            case 125:  pPDMCan->Init.Prescaler = 16;
                  break;
            case 100:  pPDMCan->Init.Prescaler = 20;
                 break;
            case 50:  pPDMCan->Init.Prescaler = 120;
                 break;
            case 20:  pPDMCan->Init.Prescaler = 300;
                 break;
            case 10:  pPDMCan->Init.Prescaler = 600;
                 break;
      }
      pPDMCan->Init.SyncJumpWidth = CAN_SJW_1TQ;
      pPDMCan->Init.TimeSeg1 = CAN_BS1_15TQ;
      pPDMCan->Init.TimeSeg2 = CAN_BS2_5TQ;

     if (HAL_CAN_Init(pPDMCan) != HAL_OK)
     {
        Error_Handler();
     }

     if (HAL_CAN_ActivateNotification(pPDMCan,
                  0
				  | CAN_IT_RX_FIFO0_MSG_PENDING | CAN_IT_RX_FIFO0_FULL | CAN_IT_RX_FIFO0_OVERRUN
				  | CAN_IT_RX_FIFO1_MSG_PENDING | CAN_IT_RX_FIFO1_FULL | CAN_IT_RX_FIFO1_OVERRUN
		  ) != HAL_OK) {
         return CO_ERROR_ILLEGAL_ARGUMENT;
      }
     HAL_NVIC_SetPriority(CAN1_RX0_IRQn, 5, 0);
     HAL_NVIC_EnableIRQ(CAN1_RX0_IRQn);
     HAL_NVIC_SetPriority(CAN1_RX1_IRQn, 5, 0);
     HAL_NVIC_EnableIRQ(CAN1_RX1_IRQn);
     HAL_NVIC_SetPriority(CAN1_SCE_IRQn, 5, 0);
     HAL_NVIC_EnableIRQ(CAN1_SCE_IRQn);

    return CO_ERROR_NO;
}


/******************************************************************************/
void CO_CANmodule_disable() {
	if (pPDMCan!= NULL)
	{
	    HAL_CAN_Stop(pPDMCan);
	}
}




uint8_t getCanFifoFree()
{
	return HAL_CAN_GetTxMailboxesFreeLevel(pPDMCan);
}



/*
 *
 */
uint8_t uPDMGetCanReady()
{
	return (HAL_CAN_GetTxMailboxesFreeLevel(pPDMCan));
}

/*
 * Функция отправики пакета СAN
 * Поскольку используется апаратный Fifo конртеллера, создание программного буфера не имеет смысла
 */
uint8_t uPDMCanSend(CAN_TX_FRAME_TYPE *buffer)
{
	uint8_t res = 0;
	static  CAN_TxHeaderTypeDef pTXHeader;
    uint32_t TxMailbox;
	//CO_LOCK_CAN_SEND(CANmodule);
    if (HAL_CAN_GetTxMailboxesFreeLevel(pPDMCan) > 0) {

    		pTXHeader.DLC                = (uint32_t)buffer->DLC;
    		if ( buffer->ident & CAN_EXT_FLAG)
    		{
    			buffer->ident 		&= ~CAN_EXT_FLAG;
    			pTXHeader.ExtId      =  buffer->ident;
    			pTXHeader.IDE        = CAN_ID_EXT;
    			pTXHeader.RTR        = (buffer->ident & FLAG_RTR) ? CAN_RTR_REMOTE : CAN_RTR_DATA;
    			pTXHeader.StdId 	 = 0U;
    		}
    		else
    		{
    			pTXHeader.ExtId              = 0U;
    	        pTXHeader.IDE                = CAN_ID_STD;
    	        pTXHeader.RTR                = (buffer->ident & FLAG_RTR) ? CAN_RTR_REMOTE : CAN_RTR_DATA;
    	        pTXHeader.StdId              = buffer->ident & CANID_MASK;

    		}

	        pTXHeader.TransmitGlobalTime = DISABLE;

	        /* Now add message to FIFO. Should not fail */
	        if (HAL_CAN_AddTxMessage(pPDMCan,  &pTXHeader, buffer->data, &TxMailbox) == HAL_OK)
	        {
	        	res = 1;
	        }
	  }

//	 CO_UNLOCK_CAN_SEND(CANmodule);
	 return res;
}



/******************************************************************************/
/* Get error counters from the module. If necessary, function may use
    * different way to determine errors. */
static uint16_t rxErrors=0, txErrors=0, overflow=0;

void CO_CANmodule_process(CO_CANmodule_t *CANmodule) {
    uint32_t err;

    err = ((uint32_t)txErrors << 16) | ((uint32_t)rxErrors << 8) | overflow;

    if (CANmodule->errOld != err) {
        uint16_t status = CANmodule->CANerrorStatus;

        CANmodule->errOld = err;

        if (txErrors >= 256U) {
            /* bus off */
            status |= CO_CAN_ERRTX_BUS_OFF;
        }
        else {
            /* recalculate CANerrorStatus, first clear some flags */
            status &= 0xFFFF ^ (CO_CAN_ERRTX_BUS_OFF |
                                CO_CAN_ERRRX_WARNING | CO_CAN_ERRRX_PASSIVE |
                                CO_CAN_ERRTX_WARNING | CO_CAN_ERRTX_PASSIVE);

            /* rx bus warning or passive */
            if (rxErrors >= 128) {
                status |= CO_CAN_ERRRX_WARNING | CO_CAN_ERRRX_PASSIVE;
            } else if (rxErrors >= 96) {
                status |= CO_CAN_ERRRX_WARNING;
            }

            /* tx bus warning or passive */
            if (txErrors >= 128) {
                status |= CO_CAN_ERRTX_WARNING | CO_CAN_ERRTX_PASSIVE;
            } else if (rxErrors >= 96) {
                status |= CO_CAN_ERRTX_WARNING;
            }

            /* if not tx passive clear also overflow */
            if ((status & CO_CAN_ERRTX_PASSIVE) == 0) {
                status &= 0xFFFF ^ CO_CAN_ERRTX_OVERFLOW;
            }
        }

        if (overflow != 0) {
            /* CAN RX bus overflow */
            status |= CO_CAN_ERRRX_OVERFLOW;
        }

        CANmodule->CANerrorStatus = status;
    }
}


/******************************************************************************/

void HAL_CAN_TxMailbox0CompleteCallback(CAN_HandleTypeDef *hcan)
{
	//CAN_SendMessage();
}
void HAL_CAN_TxMailbox1CompleteCallback(CAN_HandleTypeDef *hcan)
{
	//CAN_SendMessage();
}
void HAL_CAN_TxMailbox2CompleteCallback(CAN_HandleTypeDef *hcan)
{
	//CAN_SendMessage();
}

static void  prv_read_can_received_msg(CAN_HandleTypeDef* can, uint32_t fifo)
{
    static CAN_RxHeaderTypeDef rx;
    CAN_FRAME_TYPE rx_message;
    /* Read received message from FIFO */
    if (HAL_CAN_GetRxMessage(can, fifo, &rx, rx_message.data) != HAL_OK)
    {
        return;
    }
    //Игнорируем RTR фрайм, поскольку у него нет данных для нас.
    if ( rx.RTR !=CAN_RTR_REMOTE )
    {
    	rx_message.ident = rx.StdId;
    	rx_message.DLC = rx.DLC;
    	rx_message.filter_id = rx.FilterMatchIndex;
    	vCanInsertToRXQueue(& rx_message);
    }
    return;
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	prv_read_can_received_msg(hcan, CAN_RX_FIFO0);
	return;
}

void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	prv_read_can_received_msg(hcan, CAN_RX_FIFO1);
	return;
}

