/*
 * PDMhardware.h
 *
 *  Created on: Jun 16, 2022
 *      Author: igor.dymov
 */

#ifndef PDMHARDWARE_H_
#define PDMHARDWARE_H_

#include "main.h"
#include "FreeRTOS.h"
#include "cmsis_os.h"
#include "event_groups.h"

#define ADC1_READY         0x01U
#define ADC2_READY         0x02U
#define ADC3_READY         0x04U
#define ADC1_CHANNELS      4U
#define ADC2_CHANNELS      12U
#define ADC3_CHANNELS      8U
#define ADC_FRAME_SIZE     3U


typedef enum {
	OUT_ON,
	OUT_OFF,

} OUT_STATE_TYPE;


typedef enum {
	OUT_1 = 0,
	OUT_2 = 1,
	OUT_3 = 2,
	OUT_4 = 3,
	OUT_5 = 4,
	OUT_6 = 5,
	OUT_7 = 6,
	OUT_8 = 7,
	OUT_9 = 8,
	OUT_10 = 9,
	OUT_11 = 10,
	OUT_12 = 11,
	OUT_13 = 12,
	OUT_14 = 13,
	OUT_15 = 14,
	OUT_16 = 15,
	OUT_17 = 16,
	OUT_18 = 17,
	OUT_19 = 18,
	OUT_20 = 19,

} OUT_NAME_TYPE;


void vADC_Ready(uint8_t adc_number);
void vADCTask(void * argument);

#endif /* PDMHARDWARE_H_ */
