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

#define UID_32_LENGTH 3U

typedef struct
{
  float            current;
  float            max;
  PDM_OUT_STATE_t  state;
  ERROR_FLAGS_TYPE error;
} DOUT_TELEMETRY;
typedef struct
{
  uint32_t    time;
  LUA_STATE_t state;
  uint8_t     counter;
} LUA_TELEMETRY;
typedef struct
{
  float          battery;
  float          voltage[AIN_COUNT];
  uint8_t        din[DIN_CHANNEL];
  DOUT_TELEMETRY douts[OUT_COUNT];
  LUA_TELEMETRY  lua;
} PDM_TELEMETRY;
typedef struct
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
uint32_t ulDATAgetTelemetryLength ( void );
uint8_t  uDATAgetSystem ( uint8_t adr, uint8_t size, uint8_t* out );
uint32_t ulDATAgetSystemLength ( void );

#endif /* INC_DATA_H_ */
