/*
 * pdm_input.h
 *
 *  Created on: Jul 11, 2022
 *      Author: igor.dymov
 */

#ifndef PDM_INPUT_H_
#define PDM_INPUT_H_



#include "main.h"
#include "FreeRTOS.h"
#include "cmsis_os.h"
#include "event_groups.h"
#include "luatask.h"
#include "system.h"

#define DIN_CHANNEL 11
#define DIN_VALID   3

#define DEF_H_FRONT 10
#define DEF_L_FRONT 10


typedef struct {
	uint32_t Pin;
	GPIO_TypeDef * GPIOx;
} PIN_CONFIG;


typedef enum {
	POSITIVE_STATE = 0,
	NEGATIVE_STATE = 1,
} LOGIC_STATE;

typedef struct
{
uint32_t counter;
uint8_t data;
uint32_t low_counter;
uint32_t high_counter;
uint8_t temp_data;
LOGIC_STATE state;
} DoutCinfig;

typedef enum {
		INPUT1 = 0,
		INPUT2 = 1,
		INPUT3 = 2,
		INPUT4 = 3,
		INPUT5 = 4,
		INPUT6 = 5,
		INPUT7 = 6,
		INPUT8 = 7,
		INPUT9 = 8,
		INPUT10 = 9,
		INPUT11 = 10
} PDM_INPUT_NAME;
typedef struct {
	PDM_INPUT_NAME channel;
	uint32_t data;
} CAPTURE_DATA_TYPE;



typedef enum  {
		CONFIG_OK = 0,
		CONFIG_NO_SUPPORT = 1U,
		WRONG_CHANNEL_NUMBER =2U,
		WRONG_CHANNEL_MODE =3U,
} PDM_INPUT_CONFIG_ERROR;

void vDINconfig( void );
PDM_INPUT_CONFIG_ERROR inputConfig( uint8_t channel, LOGIC_STATE ls, uint32_t hfront, uint32_t lfront);
void vDinTask(void *argument);
void SystemDinTimer(void);
uint8_t uDinGet(PDM_INPUT_NAME channel);
#endif /* PDM_INPUT_H_ */
