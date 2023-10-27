/*
 * filters.h
 *
 *  Created on: Sep 26, 2023
 *      Author: i.dymov
 */

#ifndef FILTERS_H_
#define FILTERS_H_

#include "main.h"

typedef  uint32_t median_data;
typedef  uint32_t aver_data;
typedef  uint32_t ab_data;
#define MEDIAN_COUNT 3
typedef struct
{
  uint8_t index;
  median_data data[MEDIAN_COUNT ];
} median_filter_data_t;

typedef struct
{
	aver_data old_data;
	uint8_t adaprive;
	float k_down;
	float k_up;
} aver_filter_data_t;


typedef struct
{
	ab_data old_data;
	uint8_t k;
} ab_filter_data_t;

void vInitABFilter( ab_filter_data_t * filter, float k);
ab_data RunABFilter( ab_data data, ab_filter_data_t * filter);
void vInitMedianFilter(median_filter_data_t * filter);
median_data MedianFilter( median_data new_val,median_filter_data_t * filter);
aver_data RunAvrageFilter( aver_data data,aver_filter_data_t * filter );
void vInitRunAverga( aver_filter_data_t * filter,  float k );

#endif /* FILTERS_H_ */
