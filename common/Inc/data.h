/*
 * data.h
 *
 *  Created on: Jul 28, 2022
 *      Author: 79110
 */

#ifndef INC_DATA_H_
#define INC_DATA_H_

#include "stdint.h"

#define DATA_ADR_SIZE  10U

typedef enum {
  DATA_OK,
  DATA_ERROR_ADR
} DATA_ERROR;

typedef enum
{
  DATA_ADR_UNIQUE_0,          /* 0 */
  DATA_ADR_UNIQUE_1,          /* 1 */
  DATA_ADR_UNIQUE_2,          /* 2 */
  DATA_ADR_CURRENT_0,         /* 3 */
  DATA_ADR_CURRENT_1,         /* 4 */
  DATA_ADR_CURRENT_2,         /* 5 */
  DATA_ADR_CURRENT_3,         /* 6 */
  DATA_ADR_CURRENT_4,         /* 7 */
  DATA_ADR_CURRENT_5,         /* 8 */
  DATA_ADR_CURRENT_6,         /* 9 */
  DATA_ADR_CURRENT_7,         /* 10 */
  DATA_ADR_CURRENT_8,         /* 11 */
  DATA_ADR_CURRENT_9,         /* 12 */
  DATA_ADR_CURRENT_10,        /* 13 */
  DATA_ADR_CURRENT_11,        /* 14 */
  DATA_ADR_CURRENT_12,        /* 15 */
  DATA_ADR_CURRENT_13,        /* 16 */
  DATA_ADR_CURRENT_14,        /* 17 */
  DATA_ADR_CURRENT_15,        /* 18 */
  DATA_ADR_CURRENT_16,        /* 19 */
  DATA_ADR_CURRENT_17,        /* 20 */
  DATA_ADR_CURRENT_18,        /* 21 */
  DATA_ADR_CURRENT_19,        /* 22 */
  DATA_ADR_VOLTAGE_BAT,       /* 23 */
  DATA_ADR_VOLTAGE_0,         /* 24 */
  DATA_ADR_VOLTAGE_1,         /* 25 */
  DATA_ADR_VOLTAGE_2,         /* 26 */
  DATA_ADR_VOLTAGE_3,         /* 27 */
  DATA_ADR_DIN,               /* 28 */
  DATA_ADR_DOUT_0,            /* 29 */
  DATA_ADR_DOUT_1,            /* 30 */
  DATA_ADR_DOUT_2,            /* 31 */
  DATA_ADR_DOUT_3,            /* 32 */
  DATA_ADR_DOUT_4,            /* 33 */
  DATA_ADR_DOUT_5,            /* 34 */
  DATA_ADR_DOUT_6,            /* 35 */
  DATA_ADR_DOUT_7,            /* 36 */
  DATA_ADR_DOUT_8,            /* 37 */
  DATA_ADR_DOUT_9,            /* 38 */
  DATA_ADR_DOUT_10,           /* 39 */
  DATA_ADR_DOUT_11,           /* 40 */
  DATA_ADR_DOUT_12,           /* 41 */
  DATA_ADR_DOUT_13,           /* 42 */
  DATA_ADR_DOUT_14,           /* 43 */
  DATA_ADR_DOUT_15,           /* 44 */
  DATA_ADR_DOUT_16,           /* 45 */
  DATA_ADR_DOUT_17,           /* 46 */
  DATA_ADR_DOUT_18,           /* 47 */
  DATA_ADR_DOUT_19,           /* 48 */
  DATA_ADR_LUA_STATUS,        /* 49 */
  DATA_ADR_LUA_ERROR,         /* 50 */
  DATA_ADR_LUA_TIME,          /* 51 */
  DATA_ADR_LUA_ERROR_COUNTER  /* 52 */
} DATA_ADR;

DATA_ERROR eDATAget ( DATA_ADR adr, uint8_t* out, uint8_t* length, uint8_t size );

#endif /* INC_DATA_H_ */
