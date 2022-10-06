/*
 * PDMhardware.h
 *
 *  Created on: Jun 16, 2022
 *      Author: igor.dymov
 */

#ifndef PDMHARDWARE_H_
#define PDMHARDWARE_H_

#include "main.h"


#define AIN_COUNT			4U		//Количесвто аналоговых входов
#define OUT_COUNT           20U    //Колчество каналов
#define OUT_HPOWER_COUNT    8U     //Количесво мощных каналов
#define VELOCITY_COUNT      2U     // Количество каналов скорости
#define DEFAULT_HPOWER      59.0  // Номинальный ток по умолчания для мощных каналов
#define MAX_HPOWER			59.0  // Максимальный номинальый ток для мощных каналов
#define DEFAULT_LPOWER      8.0  // Номинальый ток маломощных каналов
#define MAX_LPOWER			8.0  //Максимальный номинальый ток для маломощных каналов
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
#define ADC1_CHANNELS      8U
#define ADC2_CHANNELS      7U
#define ADC3_CHANNELS      9U
#define ADC_FRAME_SIZE     3U
#define R2  10000
#define R1  140000

#define K   ( 3.3 / 0xFFF )
#define RR  330.0
#define K25O20   6420U
#define V25O20   (float)(25.0/K25O20*RR)
#define K10O20   6570U
#define V10O20	 (float)(10.0/K10O20*RR)
#define K06O20    6749U
#define V06O20	 (float)(6.0/K10O20*RR)
#define K0025O20  7410U
#define V0025O20  (float)(0.25/K0025O20*RR)
#define K10O08  2850U
#define V10O08	(float)(10.0/K10O08*RR)
#define K03O08  3000U
#define V03O08	(float)(3.0/K03O08*RR)
#define K02O08  3100U
#define V02O08	(float)(2.0/K02O08*RR)
#define K005O08  3150U
#define V005O08	(float)(0.5/K005O08*RR)
#define K0005O08  3350U
#define V0005O08	(float)(0.05/K0005O08*RR)
#define ERROR_CURRENT  (uint16_t)(4000U)


#define COOF  R1 / ( R1 + R2 ) * K
#define STATE_OUT_CONFIG		  0x20


typedef enum {
  STATE_OUT_OFF,
  STATE_OUT_ON_PROCESS,
  STATE_OUT_ON,
  STATE_OUT_ERROR,
  STATE_OUT_ERROR_PROCESS,
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

typedef struct
{
   uint32_t  channel;
   uint8_t   out_line_state;
   TIM_HandleTypeDef * ptim;
   OUT_STATE out_logic_state;
   PDM_OUT_STATE_t out_state;
   float power;
   float overload_power;
   uint8_t PWM;
   uint8_t ucNoRestartState;
   uint8_t error_count; //Кол-во попыток рестарта
   uint16_t overload_config_timer;
   uint16_t restart_timer;
   uint16_t restart_config_timer;
   ERROR_FLAGS_TYPE error_flag;
   LIN_COOF CSC[4];
   float current;
   GPIO_TypeDef* GPIOx;
   uint16_t GPIO_Pin;
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

void vOutInit( void );
void vOutHWDisabale(OUT_NAME_TYPE out_name);
void vOutSetState(OUT_NAME_TYPE out_name, uint8_t state);
void vADC_Ready(uint8_t adc_number);
void vADCTask(void * argument);
void vHWOutSet( OUT_NAME_TYPE out_name,  uint8_t power);
void vHWOutInit(OUT_NAME_TYPE out_name, TIM_HandleTypeDef * ptim, uint32_t  uiChannel, GPIO_TypeDef* EnablePort, uint16_t EnablePin,   uint8_t PWM);
ERROR_CODE vHWOutResetConfig(OUT_NAME_TYPE out_name, uint8_t restart_count, uint16_t timer);
ERROR_CODE vHWOutOverloadConfig(OUT_NAME_TYPE out_name,  float power, uint16_t overload_timer, float overload_power);
void vOutHWEnbale(OUT_NAME_TYPE out_name);
ERROR_CODE vOutSetPWM(OUT_NAME_TYPE out_name, uint8_t PWM);
float fOutGetCurrent(OUT_NAME_TYPE out_name);
uint16_t GetTimer(void);
PDM_OUT_STATE_t eOutGetState ( OUT_NAME_TYPE eChNum  );
float fOutGetCurrent(OUT_NAME_TYPE eChNum);
float fAinGetState(AIN_NAME_TYPE channel);
ERROR_FLAGS_TYPE eOutGetError(OUT_NAME_TYPE eChNum );
float fOutGetMaxCurrent(OUT_NAME_TYPE eChNum);
float fOutGetPrintCurrent ( OUT_NAME_TYPE eChNum);
#endif /* PDMHARDWARE_H_ */
