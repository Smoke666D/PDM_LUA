/*
 * adc_dma.h
 *
 *  Created on: 8 мар. 2023 г.
 *      Author: smoke
 */

#ifndef ADC_DMA_H_
#define ADC_DMA_H_

#include "main.h"


 typedef struct
 {
   __IO uint32_t ISR;   /*!< DMA interrupt status register */
   __IO uint32_t Reserved0;
   __IO uint32_t IFCR;  /*!< DMA interrupt flag clear register */
 } DMA_Base_Registers;

#define HAL_TIMEOUT_DMA_ABORT    5U

void DMA_IRQHandler(DMA_HandleTypeDef *hdma);
void ADC_Start_DMA(ADC_HandleTypeDef* hadc, uint32_t* pData, uint32_t Length);
void ADC_STOP();
void vADCEnable();
#endif /* ADC_DMA_H_ */
