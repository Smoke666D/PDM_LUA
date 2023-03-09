/*
 * dma_pdm.h
 *
 *  Created on: 9 мар. 2023 г.
 *      Author: igor.dymov
 */

#ifndef DMA_PDM_H_
#define DMA_PDM_H_

#include "main.h"

 typedef struct
 {
   __IO uint32_t ISR;   /*!< DMA interrupt status register */
   __IO uint32_t Reserved0;
   __IO uint32_t IFCR;  /*!< DMA interrupt flag clear register */
 } DMA_Base_Registers;

void ADC_STOP();
void DMA_IRQHandler(DMA_HandleTypeDef *hdma);
void ADC_Start_DMA(ADC_HandleTypeDef* hadc, uint32_t* pData, uint32_t Length);
void vADCEnable();
#endif /* DMA_PDM_H_ */
