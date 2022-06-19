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
#define ADC1_CHANNELS      8U
#define ADC2_CHANNELS      7U
#define ADC3_CHANNELS      9U
#define ADC_FRAME_SIZE     3U

#define R2  10000
#define R1  140000
#define RR  1000
#define K   ( 3.3 / 0xFFF )
#define COOF  R1 / ( R1 + R2 ) * K

typedef struct
{
	uint16_t KOOF;
	uint16_t Data;

} KAL_DATA;



typedef struct
{
	uint16_t data;
	float k;
	float b;
}   LIN_COOF;




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
