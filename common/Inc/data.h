/*
 * data.h
 *
 *  Created on: Jul 28, 2022
 *      Author: 79110
 */

#ifndef INC_DATA_H_
#define INC_DATA_H_

#include "stdint.h"
#include "PDMhardware.h"
#include "pdm_input.h"
#include "version.h"

#define UID_32_LENGTH       3U
#define ERROR_STRING_LENGTH 100U

typedef struct __packed
{
  float            current;
  float            max;
  PDM_OUT_STATE_t  state;
  ERROR_FLAGS_TYPE error;
} DOUT_TELEMETRY;
typedef struct __packed
{
  uint32_t    time;
  LUA_STATE_t state;
  uint8_t     counter;
} LUA_TELEMETRY; /* 6 */
typedef struct __packed
{
  float          battery;                        /* 0 */
  float          voltage[AIN_COUNT];             /* 4 */
  uint8_t        din[DIN_CHANNEL];               /* 20 */
  DOUT_TELEMETRY douts[OUT_COUNT];               /* 31 */
  LUA_TELEMETRY  lua;                            /* 231 */
  uint16_t       velocity[VELOCITY_COUNT];       /* 236 */
  float          temperature[TEMPERATURE_COUNT]; /* 240 */
  float          angle[ANGLE_COUNT];             /* 248 */
} PDM_TELEMETRY; /* 246 */
typedef struct __packed
{
  uint32_t uid[UID_32_LENGTH];
  VERSION  bootloader;
  VERSION  firmware;
  VERSION  hardware;
  VERSION  lua;
} PDM_DATA;

void     vDATAinit ( void );
void     vDATAupdate ( void );
uint8_t  uDATAgetTelemetry ( uint8_t adr, uint8_t size, uint8_t* out );
uint8_t  uDATAgetErrorString ( uint8_t adr, uint8_t size, uint8_t* out );
uint32_t ulDATAgetTelemetryLength ( void );
uint8_t  uDATAgetSystem ( uint8_t adr, uint8_t size, uint8_t* out );
uint32_t ulDATAgetSystemLength ( void );

#endif /* INC_DATA_H_ */
