/*
 * PDMhardware.h
 *
 *  Created on: Jun 16, 2022
 *      Author: igor.dymov
 */

#ifndef PDMHARDWARE_H_
#define PDMHARDWARE_H_

#include "main.h"



#define OUT_COUNT           20U    //Колчество каналов
#define OUT_HPOWER_COUNT    8U     //Количесво мощных каналов
#define DEFAULT_HPOWER      20.0  // Номинальный ток для мощных каналов
#define DEFAULT_LPOWER      8.0  // Номинальый ток маломощных каналов
#define DEFAULT_OVERLOAD_TIMER_HPOWER   1000U //Время плавного пуска для мощных каналов
#define DEFAULT_OVERLOAD_TIMER_LPOWER   0U //Время плавного пуска для маломощнвх каналов
#define DEFAULT_HPOWER_MAX              60.0 // Ток перегрузки при старте для мощных каналов
#define DEFAULT_LPOWER_MAX              10.0 // Ток перегрузки при старте для маломощных каналов
#define DEFAULT_PWM				100U

#define ADC1_READY         0x01
#define ADC2_READY         0x02
#define ADC3_READY         0x04
#define OUT_BUSY		   0x08
#define ADC1_CHANNELS      8U
#define ADC2_CHANNELS      7U
#define ADC3_CHANNELS      9U
#define ADC_FRAME_SIZE     3U
#define R2  10000
#define R1  140000
#define RR  1000.0
#define K   ( 3.3 / 0xFFF )
#define COOF  R1 / ( R1 + R2 ) * K

//Состония конечного автомата обработки входов
#define 	STATE_OUT_OFF 			  0x00
#define		STATE_OUT_ON_PROCESS      0x01
#define		STATE_OUT_ON			  0x02
#define		STATE_OUT_ERROR			  0x04
#define		STATE_OUT_ERROR_PROCESS   0x08
#define		STATE_OUT_RESTART_PROCESS 0x10
#define		STATE_OUT_CONFIG		  0x20

#define START_POWER	30U //Мощность при начала плавного пуска

//Ошибки состония выхода
typedef enum {
	ERROR_OFF,
	ERROR_ON,
	ERROR_OVERLOAD,
} ERROR_FLAGS_TYPE;

//Логические стостония выхода
typedef enum {
	OUT_ON,
	OUT_OFF,
	OUT_RESTART,
} OUT_STATE;

//Коофиценты для расчета функции зависимости тока на выходе ISENSE ключей
typedef struct
{
	uint16_t data;
	float k;
	float b;
}   LIN_COOF;


typedef struct
{
   uint32_t  channel;
   TIM_HandleTypeDef * ptim;
   OUT_STATE out_logic_state;
   uint8_t out_state;
   float power;
   float overload_power;
   uint8_t PWM;
   uint16_t overload_config_timer;
   uint16_t overload_timer;
   uint8_t error_count; //Кол-во попыток рестарта
   uint16_t restart_timer;
   uint16_t restart_config_timer;
   ERROR_FLAGS_TYPE error_flag;
   LIN_COOF CSC[4];
   float current;
} PDM_OUTPUT_TYPE;




typedef struct
{
	uint16_t KOOF;
	float Data;
} KAL_DATA;





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

void vOutState(OUT_NAME_TYPE out_name, uint8_t state);
void vADC_Ready(uint8_t adc_number);
void vADCTask(void * argument);
void vOutContolTask(void * argument);
void vOutContolTask(void * argument);
void vHWOutResete( OUT_NAME_TYPE out_name);
void vHWOutSet( OUT_NAME_TYPE out_name,  uint8_t power);
void vHWOutInit( OUT_NAME_TYPE out_name, TIM_HandleTypeDef * ptim, uint32_t  channel,  float power, uint16_t overload_timer, float overload_power, uint8_t PWM);
void vHWOutResetConfig(OUT_NAME_TYPE out_name, uint8_t restart_count, uint16_t timer);
void vHWOutOverloadConfig(OUT_NAME_TYPE out_name,  float power, uint16_t overload_timer, float overload_power);
void vOutSetPWM(OUT_NAME_TYPE out_name, uint8_t PWM);
void vHWOutResetConfig(OUT_NAME_TYPE out_name, uint8_t restart_count, uint16_t timer);
#endif /* PDMHARDWARE_H_ */
