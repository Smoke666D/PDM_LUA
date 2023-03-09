/*
 * dma_pdm.c
 *
 *  Created on: 9 мар. 2023 г.
 *      Author: igor.dymov
 */

#include "dma_pdm.h"

extern ADC_HandleTypeDef hadc1;
extern ADC_HandleTypeDef hadc2;
extern ADC_HandleTypeDef hadc3;


 static void ADC_DMAError(DMA_HandleTypeDef *hdma);
/*
 *
 */
 void ADC_Start_DMA(ADC_HandleTypeDef* hadc, uint32_t* pData, uint32_t Length)
 {

   CLEAR_BIT(hadc->Instance->CR2, ADC_CR2_DMA);
   if(HAL_IS_BIT_SET(hadc->Instance->CR2, ADC_CR2_ADON))
   {
     ADC_STATE_CLR_SET(hadc->State, HAL_ADC_STATE_READY | HAL_ADC_STATE_REG_EOC | HAL_ADC_STATE_REG_OVR, HAL_ADC_STATE_REG_BUSY);
     __HAL_ADC_CLEAR_FLAG(hadc, ADC_FLAG_EOC | ADC_FLAG_OVR);
     __HAL_ADC_ENABLE_IT(hadc, ADC_IT_OVR);
     hadc->Instance->CR2 |= ADC_CR2_DMA;

     DMA_Base_Registers *regs = (DMA_Base_Registers *)hadc->DMA_Handle->StreamBaseAddress;


      if(HAL_DMA_STATE_READY == hadc->DMA_Handle->State)
      {
        	hadc->DMA_Handle->State = HAL_DMA_STATE_BUSY;
        	hadc->DMA_Handle->ErrorCode = HAL_DMA_ERROR_NONE;
            /* Clear DBM bit */
        	hadc->DMA_Handle->Instance->CR &= (uint32_t)(~DMA_SxCR_DBM);
        	hadc->DMA_Handle->Instance->NDTR = Length;
        	hadc->DMA_Handle->Instance->PAR = &hadc->Instance->DR;
        	hadc->DMA_Handle->Instance->M0AR = pData;
        	regs->IFCR = 0x3FU << hadc->DMA_Handle->StreamIndex;
        	hadc->DMA_Handle->Instance->CR  |= DMA_IT_TC | DMA_IT_TE | DMA_IT_DME;
        	__HAL_DMA_ENABLE(hadc->DMA_Handle);
        }
     hadc->Instance->CR2 |= (uint32_t)ADC_CR2_SWSTART;
   }
   else
   {
     SET_BIT(hadc->State, HAL_ADC_STATE_ERROR_INTERNAL);
     SET_BIT(hadc->ErrorCode, HAL_ADC_ERROR_INTERNAL);
   }

 }

#define HAL_TIMEOUT_DMA_ABORT    5U
 /* Private types -------------------------------------------------------------*/



 /*
  *
  */
  static void ADC_DMAError(DMA_HandleTypeDef *hdma)
  {
    ADC_HandleTypeDef* hadc = ( ADC_HandleTypeDef* )((DMA_HandleTypeDef* )hdma)->Parent;
    hadc->State= HAL_ADC_STATE_ERROR_DMA;
    hadc->ErrorCode |= HAL_ADC_ERROR_DMA;
  }

  static void ADC_DMAConvCplt(DMA_HandleTypeDef *hdma)
  {
    /* Retrieve ADC handle corresponding to current DMA handle */
    ADC_HandleTypeDef* hadc = ( ADC_HandleTypeDef* )((DMA_HandleTypeDef* )hdma)->Parent;

    /* Update state machine on conversion status if not in error state */
    if (HAL_IS_BIT_CLR(hadc->State, HAL_ADC_STATE_ERROR_INTERNAL | HAL_ADC_STATE_ERROR_DMA))
    {
      /* Update ADC state machine */
      SET_BIT(hadc->State, HAL_ADC_STATE_REG_EOC);

      /* Determine whether any further conversion upcoming on group regular   */
      /* by external trigger, continuous mode or scan sequence on going.      */
      /* Note: On STM32F4, there is no independent flag of end of sequence.   */
      /*       The test of scan sequence on going is done either with scan    */
      /*       sequence disabled or with end of conversion flag set to        */
      /*       of end of sequence.                                            */
      if(ADC_IS_SOFTWARE_START_REGULAR(hadc)                   &&
         (HAL_IS_BIT_CLR(hadc->Instance->SQR1, ADC_SQR1_L) ||
          HAL_IS_BIT_CLR(hadc->Instance->CR2, ADC_CR2_EOCS)  )   )
      {
        /* Disable ADC end of single conversion interrupt on group regular */
        /* Note: Overrun interrupt was enabled with EOC interrupt in          */
        /* HAL_ADC_Start_IT(), but is not disabled here because can be used   */
        /* by overrun IRQ process below.                                      */
        __HAL_ADC_DISABLE_IT(hadc, ADC_IT_EOC);

        /* Set ADC state */
        CLEAR_BIT(hadc->State, HAL_ADC_STATE_REG_BUSY);

        if (HAL_IS_BIT_CLR(hadc->State, HAL_ADC_STATE_INJ_BUSY))
        {
          SET_BIT(hadc->State, HAL_ADC_STATE_READY);
        }
      }
    }
    else /* DMA and-or internal error occurred */
    {
      if ((hadc->State & HAL_ADC_STATE_ERROR_INTERNAL) != 0UL)
      {
        /* Call HAL ADC Error Callback function */
      	HAL_ADC_ErrorCallback(hadc);
      }
  	else
  	{
        /* Call DMA error callback */
  		ADC_DMAError(hdma);
      }
    }
  }

 /*
  *
  */
 HAL_StatusTypeDef DMA_Abort(DMA_HandleTypeDef *hdma)
 {
   /* calculate DMA base and stream number */
   DMA_Base_Registers *regs = (DMA_Base_Registers *)hdma->StreamBaseAddress;

   uint32_t tickstart = HAL_GetTick();

   if(hdma->State != HAL_DMA_STATE_BUSY)
   {
     hdma->ErrorCode = HAL_DMA_ERROR_NO_XFER;
     return HAL_ERROR;
   }
   else
   {
     /* Disable all the transfer interrupts */
     hdma->Instance->CR  &= ~(DMA_IT_TC | DMA_IT_TE | DMA_IT_DME);
     hdma->Instance->FCR &= ~(DMA_IT_FE);


     /* Disable the stream */
     __HAL_DMA_DISABLE(hdma);

     /* Check if the DMA Stream is effectively disabled */
     while((hdma->Instance->CR & DMA_SxCR_EN) != RESET)
     {
       /* Check for the Timeout */
       if((HAL_GetTick() - tickstart ) > HAL_TIMEOUT_DMA_ABORT)
       {
         /* Update error code */
         hdma->ErrorCode = HAL_DMA_ERROR_TIMEOUT;

         /* Change the DMA state */
         hdma->State = HAL_DMA_STATE_TIMEOUT;

         return HAL_TIMEOUT;
       }
     }

     /* Clear all interrupt flags at correct offset within the register */
     regs->IFCR = 0x3FU << hdma->StreamIndex;

     /* Change the DMA state*/
     hdma->State = HAL_DMA_STATE_READY;

   }
   return HAL_OK;
 }


 void ADC_STOP()
 {
	   __HAL_ADC_DISABLE(&hadc1);
	   __HAL_ADC_DISABLE(&hadc2);
	   __HAL_ADC_DISABLE(&hadc3);
	   hadc1.Instance->CR2 &= ~ADC_CR2_DMA;
	   hadc2.Instance->CR2 &= ~ADC_CR2_DMA;
	   hadc3.Instance->CR2 &= ~ADC_CR2_DMA;
	   if (hadc1.DMA_Handle->State == HAL_DMA_STATE_BUSY)
	   {
	        if (DMA_Abort(hadc1.DMA_Handle) != HAL_OK)
	        {
	          SET_BIT(hadc1.State, HAL_ADC_STATE_ERROR_DMA);
	        }
	   }
	   if (hadc2.DMA_Handle->State == HAL_DMA_STATE_BUSY)
		   {
		        if (DMA_Abort(hadc2.DMA_Handle) != HAL_OK)
		        {
		          SET_BIT(hadc2.State, HAL_ADC_STATE_ERROR_DMA);
		        }
		   }
	   if (hadc3.DMA_Handle->State == HAL_DMA_STATE_BUSY)
		   {
		        if (DMA_Abort(hadc3.DMA_Handle) != HAL_OK)
		        {
		          SET_BIT(hadc3.State, HAL_ADC_STATE_ERROR_DMA);
		        }
		   }
	   __HAL_ADC_DISABLE_IT(&hadc1, ADC_IT_OVR);
	   __HAL_ADC_DISABLE_IT(&hadc2, ADC_IT_OVR);
	   __HAL_ADC_DISABLE_IT(&hadc3, ADC_IT_OVR);
	      ADC_STATE_CLR_SET(hadc1.State,
	                        HAL_ADC_STATE_REG_BUSY | HAL_ADC_STATE_INJ_BUSY,
	                        HAL_ADC_STATE_READY);
	      ADC_STATE_CLR_SET(hadc2.State,
	      	                        HAL_ADC_STATE_REG_BUSY | HAL_ADC_STATE_INJ_BUSY,
	      	                        HAL_ADC_STATE_READY);
	      ADC_STATE_CLR_SET(hadc3.State,
	      	                        HAL_ADC_STATE_REG_BUSY | HAL_ADC_STATE_INJ_BUSY,
	      	                        HAL_ADC_STATE_READY);

 }

 void DMA_IRQHandler(DMA_HandleTypeDef *hdma)
 {
   uint32_t tmpisr;
   __IO uint32_t count = 0U;
   uint32_t timeout = SystemCoreClock / 9600U;

   /* calculate DMA base and stream number */
   DMA_Base_Registers *regs = (DMA_Base_Registers *)hdma->StreamBaseAddress;

   tmpisr = regs->ISR;

   /* Transfer Error Interrupt management ***************************************/
   if ((tmpisr & (DMA_FLAG_TEIF0_4 << hdma->StreamIndex)) != RESET)
   {
     if(__HAL_DMA_GET_IT_SOURCE(hdma, DMA_IT_TE) != RESET)
     {
       /* Disable the transfer error interrupt */
       hdma->Instance->CR  &= ~(DMA_IT_TE);

       /* Clear the transfer error flag */
       regs->IFCR = DMA_FLAG_TEIF0_4 << hdma->StreamIndex;

       /* Update error code */
       hdma->ErrorCode |= HAL_DMA_ERROR_TE;
     }
   }
   /* FIFO Error Interrupt management ******************************************/
   if ((tmpisr & (DMA_FLAG_FEIF0_4 << hdma->StreamIndex)) != RESET)
   {
     if(__HAL_DMA_GET_IT_SOURCE(hdma, DMA_IT_FE) != RESET)
     {
       /* Clear the FIFO error flag */
       regs->IFCR = DMA_FLAG_FEIF0_4 << hdma->StreamIndex;

       /* Update error code */
       hdma->ErrorCode |= HAL_DMA_ERROR_FE;
     }
   }
   /* Direct Mode Error Interrupt management ***********************************/
   if ((tmpisr & (DMA_FLAG_DMEIF0_4 << hdma->StreamIndex)) != RESET)
   {
     if(__HAL_DMA_GET_IT_SOURCE(hdma, DMA_IT_DME) != RESET)
     {
       /* Clear the direct mode error flag */
       regs->IFCR = DMA_FLAG_DMEIF0_4 << hdma->StreamIndex;

       /* Update error code */
       hdma->ErrorCode |= HAL_DMA_ERROR_DME;
     }
   }

   /* Transfer Complete Interrupt management ***********************************/
   if ((tmpisr & (DMA_FLAG_TCIF0_4 << hdma->StreamIndex)) != RESET)
   {
     if(__HAL_DMA_GET_IT_SOURCE(hdma, DMA_IT_TC) != RESET)
     {
       /* Clear the transfer complete flag */
       regs->IFCR = DMA_FLAG_TCIF0_4 << hdma->StreamIndex;

       if(HAL_DMA_STATE_ABORT == hdma->State)
       {
         /* Disable all the transfer interrupts */
         hdma->Instance->CR  &= ~(DMA_IT_TC | DMA_IT_TE | DMA_IT_DME);
         hdma->Instance->FCR &= ~(DMA_IT_FE);

         /* Clear all interrupt flags at correct offset within the register */
         regs->IFCR = 0x3FU << hdma->StreamIndex;
         /* Change the DMA state */
         hdma->State = HAL_DMA_STATE_READY;
         return;
       }

       if(((hdma->Instance->CR) & (uint32_t)(DMA_SxCR_DBM)) != RESET)
       {
    	   ADC_DMAConvCplt(hdma);

       }
       /* Disable the transfer complete interrupt if the DMA mode is not CIRCULAR */
       else
       {
         if((hdma->Instance->CR & DMA_SxCR_CIRC) == RESET)
         {
           /* Disable the transfer complete interrupt */
           hdma->Instance->CR  &= ~(DMA_IT_TC);

           /* Change the DMA state */
           hdma->State = HAL_DMA_STATE_READY;
         }
         ADC_DMAConvCplt(hdma);

       }
     }
   }

   /* manage error case */
   if(hdma->ErrorCode != HAL_DMA_ERROR_NONE)
   {
     if((hdma->ErrorCode & HAL_DMA_ERROR_TE) != RESET)
     {
       hdma->State = HAL_DMA_STATE_ABORT;

       /* Disable the stream */
       __HAL_DMA_DISABLE(hdma);

       do
       {
         if (++count > timeout)
         {
           break;
         }
       }
       while((hdma->Instance->CR & DMA_SxCR_EN) != RESET);

       /* Change the DMA state */
       hdma->State = HAL_DMA_STATE_READY;

     }
     ADC_DMAConvCplt(hdma);
   }
 }


void vADCEnable()
 {
	     __HAL_ADC_ENABLE(&hadc1);
	     __HAL_ADC_ENABLE(&hadc2);
	     __HAL_ADC_ENABLE(&hadc3);
 }
