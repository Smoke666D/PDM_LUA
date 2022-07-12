/*
 * pdm_input.h
 *
 *  Created on: Jul 11, 2022
 *      Author: igor.dymov
 */

#ifndef PDM_INPUT_H_
#define PDM_INPUT_H_

#include "main.h"
#include "cmsis_os.h"
#include "stm32f4xx_hal.h"

#define DIN_CHANNEL 12
#define DIN_VALID   3
#include "cmsis_os.h"

typedef enum  {
		DIGITAL = 0,
		CAPTURE  = 1,
} PDM_INPUT_MODE;

typedef struct {
	uint32_t Pin;
	GPIO_TypeDef * GPIOx;
} PIN_CONFIG;

typedef struct {
PIN_CONFIG pin_data;
PDM_INPUT_MODE  mode;
uint8_t counter;
uint16_t data;
uint8_t temp_data;
} DoutCinfig;

typedef enum {
		INPUT1 = 1,
		INPUT2 = 2,
		INPUT3 = 3,
		INPUT4 = 4,
		INPUT5 = 5,
		INPUT6 = 6,
		INPUT7 = 7,
		INPUT8 = 8,
		INPUT9 = 9,
		INPUT10 = 10,
		INPUT11 = 11,
		INPUT12 = 12,

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

PDM_INPUT_CONFIG_ERROR inputConfig( PDM_INPUT_NAME channel, PDM_INPUT_MODE mode);
void vDinTask(void *argument);
#endif /* PDM_INPUT_H_ */
