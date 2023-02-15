/*
 * PDMhardware.h
 *
 *  Created on: Jun 16, 2022
 *      Author: igor.dymov
 */

#ifndef PDMHARDWARE_H_
#define PDMHARDWARE_H_

#include "main.h"
#include "luatask.h"


#define REV_2 				1

#define CS_ENABLE	   		GPIO_PIN_SET
#define CS_DISABLE	   		GPIO_PIN_RESET


#define AIN_COUNT			3U		//Количесвто аналоговых входов
#define OUT_COUNT           20U    //Колчество каналов
#define OUT_HPOWER_COUNT    8U     //Количесво мощных каналов
#define VELOCITY_COUNT      2U     // Количество каналов скорости
#define TEMPERATURE_COUNT   1U     /* Количество каналов температуры */
#define ANGLE_COUNT         2U     /* Количество каналов углов */
#define DEFAULT_HPOWER      59.0  // Номинальный ток по умолчания для мощных каналов
#define MAX_HPOWER			59.0  // Максимальный номинальый ток для мощных каналов
#define MAX_HOVERLOAD_POWER 60.0
#define DEFAULT_LPOWER      8.0  // Номинальый ток маломощных каналов
#define MAX_LPOWER			10.0  //Максимальный номинальый ток для маломощных каналов
#define MAX_LOVERLOAD_POWER 20.0
#define DEFAULT_OVERLOAD_TIMER_HPOWER   1000U //Время плавного пуска для мощных каналов
#define MAX_OVERLOAD_TIMER             32767U //Максимальное время плавного пуска для мощных каналов
#define DEFAULT_OVERLOAD_TIMER_LPOWER   0U //Время плавного пуска для маломощнвх каналов
#define DEFAULT_HPOWER_MAX              60.0 // Ток перегрузки при старте для мощных каналов
#define MAX_OVERLOAD_HPOWER             60.0 // Максиальный пусковой ток мощных каналов
#define DEFAULT_LPOWER_MAX              10.0 // Ток перегрузки при старте для маломощных каналов
#define MAX_OVERLOAD_LPOWER             10.0 // Максиальный пусковой ток маломощных каналов
#define DEFAULT_PWM				100U
#define MAX_PWM					100U
#define MAX_POWER				100U
#define DEFAULT_RESET_TIMER		1000U
#define DEFAULT_RESET_COUNTER	1U
#define MAX_RESET_TIMER         32767U

#define ADC1_READY         0x01
#define ADC2_READY         0x02
#define ADC3_READY         0x04
#define OUT_BUSY		   0x08
#define ADC_DATA_READY     0x10
#define ADC1_CHANNELS      9U
#define ADC2_CHANNELS      7U
#define ADC3_CHANNELS      9U
#define ADC_FRAME_SIZE     3U
#define R1  10000.0
#define R2  3000.0
#define R3  1500.0
#define RA1  60400.0
#define RA2  3000.0
#define RA3  10000.0

#define AINCOOF1  ( ( RA1 + RA3 ) /RA3) * K
#define INDIOD  0.2

#define K   ( 3.3 / 0xFFF )




#ifdef REV_2
	#define RR  300.0
#else
	#define RR  330.0
#endif
#define K15O20   16450U
#define V15O20   (float)(15.0/K15O20*RR)
#define K10O20   16450U
#define V10O20	 (float)(10.0/K10O20*RR)
#define K01O20    16650U
#define V01O20	 (float)(1.0/K01O20*RR)
#define K002O20  16800U
#define V002O20  (float)(0.20/K002O20*RR)

#define K30O08  15520U
#define V30O08	(float)(30.0/K30O08*RR)
#define K15O08  16910U
#define V15O08	(float)(15.0/K15O08*RR)
#define K10O08  16110U
#define V10O08	(float)(10.0/K10O08*RR)
#define K05O08  15520U
#define V05O08	(float)(5.0/K05O08*RR)
#define ERROR_CURRENT  (uint16_t)(4000U)

#define CIRCUT_BREAK_CURRENT  0.1
#define COOF  ( ( R1 + R2 ) /R2) * K
#define COOF1  ( ( R1 + R3 ) /R3) * K

#define STATE_OUT_CONFIG		  0x20

#define KOOF_COUNT 4

typedef enum {
 ADC_DMA1 =0,
 ADC_DMA2 =1,
 ADC_DMA3 =2
} ADC_DMA_NAME;


 typedef struct
 {
   __IO uint32_t ISR;   /*!< DMA interrupt status register */
   __IO uint32_t Reserved0;
   __IO uint32_t IFCR;  /*!< DMA interrupt flag clear register */
 } DMA_Base_Registers;

typedef enum {
  STATE_OUT_OFF,
  STATE_OUT_ON_PROCESS,
  STATE_OUT_ON,
  STATE_OUT_ERROR,
  STATE_OUT_RESTART_PROCESS
} PDM_OUT_STATE_t;

#define START_POWER	30U //Мощность при начала плавного пуска
#define MAX_POWER	100U //Мощность при начала плавного пуска

//Результаты выполнения функций
typedef enum {
  ERROR_OK = 0,
  INVALID_ARG = 1,
} ERROR_CODE;

//Ошибки состония выхода
typedef enum {
	ERROR_OFF,
	ERROR_OVER_LIMIT,
	ERROR_OVERLOAD,
	ERROR_CIRCUT_BREAK,
	ERROR_VDD_SHORTCUT,
	ERROR_VSS_SHORTCUT
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
	float data;
	float k;
	float b;
}   LIN_COOF;

typedef struct __packed
{
   GPIO_TypeDef* GPIOx;
   TIM_HandleTypeDef * ptim;
   uint32_t  channel;
   float power;
   float overload_power;
   float current;
   ENABLE_t EnableFlag;
   uint8_t error_counter;
   uint8_t PWM;
   uint8_t soft_start_power;
   uint16_t GPIO_Pin;
   uint16_t soft_start_timer;
   uint16_t overload_config_timer;
   uint16_t restart_timer;
   uint16_t restart_config_timer;
   ERROR_FLAGS_TYPE error_flag;
   PDM_OUT_STATE_t out_state;
   LIN_COOF CSC[KOOF_COUNT -1 ];
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

typedef enum {
	AIN_1 = 0,
	AIN_2 = 1,
	AIN_3 = 2,
	AIN_4 = 3,
} AIN_NAME_TYPE;

typedef enum {
  ANGLE_TYPE_ROLL,
  ANGLE_TYPE_PITCH,
  ANGLE_TYPE_YAW
} ANGLE_TYPE;

void vGetDoutStatus(uint32_t * Dout1_10Status, uint32_t * Dout11_20Status);
void vOutInit( void );
void vOutSetState(OUT_NAME_TYPE out_name, uint8_t state);
void vADC_Ready(uint8_t adc_number);
void vADCTask(void * argument);
ERROR_CODE vHWOutResetConfig(OUT_NAME_TYPE out_name, uint8_t restart_count, uint16_t timer);
ERROR_CODE vHWOutOverloadConfig(OUT_NAME_TYPE out_name,  float power, uint16_t overload_timer, float overload_power);
void vOutHWEnbale(OUT_NAME_TYPE out_name);
ERROR_CODE vOutSetPWM(OUT_NAME_TYPE out_name, uint8_t PWM);
float fOutGetCurrent(OUT_NAME_TYPE out_name);
PDM_OUT_STATE_t eOutGetState ( OUT_NAME_TYPE eChNum  );
float fOutGetCurrent(OUT_NAME_TYPE eChNum);
float fAinGetState ( AIN_NAME_TYPE channel );
float fBatteryGet ( void );
float fTemperatureGet ( uint8_t chanel );
ERROR_CODE vOutSetSoftStart(OUT_NAME_TYPE out_name, uint16_t timer, uint8_t power);
ERROR_FLAGS_TYPE eOutGetError(OUT_NAME_TYPE eChNum );
float fOutGetMaxCurrent(OUT_NAME_TYPE eChNum);
float fOutGetPrintCurrent ( OUT_NAME_TYPE eChNum);
void DMA_IRQHandler(DMA_HandleTypeDef *hdma);
#endif /* PDMHARDWARE_H_ */
