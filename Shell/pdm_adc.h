/*
 * pdm_adc.h
 *
 *  Created on: 8 мар. 2023 г.
 *      Author: smoke
 */

#ifndef PDM_ADC_H_
#define PDM_ADC_H_

#include "main.h"
#include "system.h"


#define AIN_COUNT			3U		//Количесвто аналоговых входов
#define MAX_CAL_POINT  20
#define MAX_TOTAL_CAL_POINT  ( AIN_COUNT * MAX_CAL_POINT  ) //Суммароне количество калибровочных точек
#define MAX_COOF_COUNT       ( AIN_COUNT * ( MAX_CAL_POINT -1 ) )
typedef enum {
  AIN1 = 0,
  AIN2 = 1,
  AIN3 = 2,
  AIN4 = 3,
  AIN5 = 4,
  AIN6 = 5,
  AIN7 = 6,
  AIN8 = 7,
  AIN9 = 8,
  AIN10 = 9,
  AIN11 = 10,
  AIN12 = 11,
  AIN13 = 12,
  AIN14 = 13,
  AIN15 = 14,
  AIN16 = 15,
  AIN17 = 16,
} AIN_NAME_t;

/*Коофиценты для кривых */
typedef struct
{
	float data;
	float k;
	float b;
}   LIN_COOF;

typedef struct
{
	uint16_t KOOF;
	float Data;
} KAL_DATA;


typedef struct
{
	float Y;
	float X;
} POINT_t;

typedef  struct
{
	uint8_t 	coof_count;
	uint8_t 	index;

} AIN_DATA_t;

typedef enum {
 CAL_SUCCESS= 0,
 CAL_MEMORY_FULL = 1,
 CAL_OVERWRITE_ERROR = 2,
 CAL_POINT_COUNT_ERROR = 3
} CAL_ERROR_CODE;

CAL_ERROR_CODE  eAinCalDataConfig(AIN_NAME_t name, uint8_t cal_point_count );
CAL_ERROR_CODE  eSetAinCalPoint(AIN_NAME_t name, POINT_t * cal_point, uint16_t PointNumber );
CAL_ERROR_CODE  eSetAinCal(AIN_NAME_t name, POINT_t * cal_point, uint8_t cal_point_count);
void vABLineKoofFinde(float * k, float * b,  float x1, float x2, float y1, float y2);
float fGetAinCalData( AIN_NAME_t name, float raw_data);

#endif /* PDM_ADC_H_ */
