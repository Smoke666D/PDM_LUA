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
				  | CAN_IT_ERROR_WARNING |  CAN_IT_ERROR_PASSIVE      | CAN_IT_BUSOFF
				  | CAN_IT_LAST_ERROR_CODE | CAN_IT_ERROR
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




static HAL_StatusTypeDef CAN_AddTxMessage(CAN_HandleTypeDef *hcan, CAN_TxHeaderTypeDef *pHeader, uint8_t aData[])
{
  uint32_t transmitmailbox;
  HAL_CAN_StateTypeDef state = hcan->State;
  uint32_t tsr = READ_REG(hcan->Instance->TSR);


  if ((state == HAL_CAN_STATE_READY) || (state == HAL_CAN_STATE_LISTENING))
  {

      /* Select an empty transmit mailbox */
      transmitmailbox = (tsr & CAN_TSR_CODE) >> CAN_TSR_CODE_Pos;

      /* Check transmit mailbox value */
      if (transmitmailbox <= 2U)
      {


    	  /* Set up the Id */
    	  if (pHeader->IDE == CAN_ID_STD)
    	  {
    		 hcan->Instance->sTxMailBox[transmitmailbox].TIR = ((pHeader->StdId << CAN_TI0R_STID_Pos) |
                                                           pHeader->RTR);
    	  }
    	  else
    	  {
    		  hcan->Instance->sTxMailBox[transmitmailbox].TIR = ((pHeader->ExtId << CAN_TI0R_EXID_Pos) |
                                                           pHeader->IDE |
                                                           pHeader->RTR);
    	  }

    	  hcan->Instance->sTxMailBox[transmitmailbox].TDTR = (pHeader->DLC);

    	  if (pHeader->TransmitGlobalTime == ENABLE)
    	  {
    		  	  SET_BIT(hcan->Instance->sTxMailBox[transmitmailbox].TDTR, CAN_TDT0R_TGT);
    	  }

    	  WRITE_REG(hcan->Instance->sTxMailBox[transmitmailbox].TDHR,
                ((uint32_t)aData[7] << CAN_TDH0R_DATA7_Pos) |
                ((uint32_t)aData[6] << CAN_TDH0R_DATA6_Pos) |
                ((uint32_t)aData[5] << CAN_TDH0R_DATA5_Pos) |
                ((uint32_t)aData[4] << CAN_TDH0R_DATA4_Pos));
    	  WRITE_REG(hcan->Instance->sTxMailBox[transmitmailbox].TDLR,
                ((uint32_t)aData[3] << CAN_TDL0R_DATA3_Pos) |
                ((uint32_t)aData[2] << CAN_TDL0R_DATA2_Pos) |
                ((uint32_t)aData[1] << CAN_TDL0R_DATA1_Pos) |
                ((uint32_t)aData[0] << CAN_TDL0R_DATA0_Pos));


    	  SET_BIT(hcan->Instance->sTxMailBox[transmitmailbox].TIR, CAN_TI0R_TXRQ);

    	  /* Return function status */
    	  return HAL_OK;
    }
  }
  return HAL_ERROR;
}


/*
 * Функция отправики пакета СAN
 * Поскольку используется апаратный Fifo конртеллера, создание программного буфера не имеет смысла
 */
uint8_t uPDMCanSend(CAN_TX_FRAME_TYPE *buffer)
{
	static  CAN_TxHeaderTypeDef pTXHeader;
	/*
	 * Проверям что какой-то есть хотя бы один свободный TX mailbox
	 */
    if ( ( ((pPDMCan->Instance->TSR & CAN_TSR_TME0) != 0U) || ((pPDMCan->Instance->TSR & CAN_TSR_TME1) != 0U) || ((pPDMCan->Instance->TSR & CAN_TSR_TME2) != 0U) ))
    {
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
    			pTXHeader.ExtId       = 0U;
    	        pTXHeader.IDE         = CAN_ID_STD;
    	        pTXHeader.RTR         = (buffer->ident & FLAG_RTR) ? CAN_RTR_REMOTE : CAN_RTR_DATA;
    	        pTXHeader.StdId       = buffer->ident & CANID_MASK;
    		}
	        pTXHeader.TransmitGlobalTime = DISABLE;

	        /* Now add message to FIFO. Should not fail */
	        if (CAN_AddTxMessage(pPDMCan,  &pTXHeader, buffer->data) == HAL_OK)
	        {
	        	return 1;
	        }
	  }
	 return 0;
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



