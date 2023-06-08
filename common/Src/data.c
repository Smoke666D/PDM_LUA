/*
 * data.c
 *
 *  Created on: Jul 28, 2022
 *      Author: 79110
 */
#include "data.h"
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include "common.h"
#include "stm32f4xx_hal.h"
#include "luatask.h"
#include "lua.h"
#include "mems.h"

#include "pdm_input.h"

static PDM_TELEMETRY telemetry  = { 0U };
static PDM_DATA      systemData = { 0U };

static uint8_t nullString[ERROR_STRING_LENGTH] = { 0U };


void vDATAinit ( void )
{
  systemData.uid[0U]          = HAL_GetUIDw0();
  systemData.uid[1U]          = HAL_GetUIDw1();
  systemData.uid[2U]          = HAL_GetUIDw2();
  systemData.firmware.major   = FIRMWARE_VERSION_MAJOR;
  systemData.firmware.minor   = FIRMWARE_VERSION_MINOR;
  systemData.firmware.patch   = FIRMWARE_VERSION_PATCH;
  systemData.hardware.major   = HARDWARE_VERSION_MAJOR;
  systemData.hardware.minor   = HARDWARE_VERSION_MINOR;
  systemData.hardware.patch   = HARDWARE_VERSION_PATCH;
  systemData.bootloader.major = 0U;
  systemData.bootloader.minor = 0U;
  systemData.bootloader.patch = 0U;
  systemData.lua.major        = atoi( LUA_VERSION_MAJOR );
  systemData.lua.minor        = atoi( LUA_VERSION_MINOR );
  systemData.lua.patch        = atoi( LUA_VERSION_RELEASE );
  return;
}

void vDATAupdate ( void )
{
  telemetry.battery = fBatteryGet();
  for ( uint8_t i=0U; i<AIN_COUNT; i++ )
  {
    telemetry.voltage[i] = fAinGetState( ( AIN_NAME_TYPE )i );
  }
  for ( uint8_t i=0U; i<DIN_CHANNEL; i++ )
  {
    telemetry.din[i] = ucDinGet( ( DIN_INPUT_NAME )i );
  }
  for ( uint8_t i=0U; i<OUT_COUNT; i++ )
  {
    telemetry.douts[i].current = fOutGetCurrent( i );
    telemetry.douts[i].max     = fOutGetMaxCurrent( i );
    telemetry.douts[i].state   = eOutGetState( i );
    telemetry.douts[i].error   = eOutGetError( i );
  }
  telemetry.lua.counter  = ucLUAgetErrorCount();
  telemetry.lua.time     = ulLUAgetWorkCicle();
  telemetry.lua.state    = eLUAgetSTATE();
  telemetry.velocity[0U] = uGetRPM1();
  telemetry.velocity[1U] = uGetRPM2();
  for ( uint8_t i=0U; i<TEMPERATURE_COUNT; i++ )
  {
    telemetry.temperature[i] = fTemperatureGet( i );
  }
  for ( uint8_t i=0U; i<ANGLE_COUNT; i++ )
  {
    telemetry.angle[i] = fAngleGet( ( ANGLE_TYPE )i );
  }
  return;
}

static uint8_t uDATAget ( uint16_t adr, uint8_t size, uint8_t* out, uint8_t* data, uint32_t dataSize )
{
  uint8_t* source = &( data )[adr];
  uint32_t remain = dataSize - adr;
  uint8_t  length = ( remain > size ) ? size : ( uint8_t )remain;
  ( void )memcpy( out, source, length );
  return length;
}

uint32_t ulDATAgetTelemetryLength ( void )
{
  return sizeof( PDM_TELEMETRY );
}
uint32_t ulDATAgetSystemLength ( void )
{
  return sizeof( PDM_DATA );
}
uint8_t uDATAgetTelemetry ( uint16_t adr, uint8_t size, uint8_t* out )
{
  return uDATAget( adr, size, out, ( uint8_t* )&telemetry, sizeof( PDM_TELEMETRY ) );
}
uint8_t uDATAgetErrorString ( uint8_t adr, uint8_t size, uint8_t* out )
{
  uint8_t  len = ERROR_STRING_LENGTH;
  uint8_t* src = nullString;
  if ( pcGetLUAError() != NULL ) {
    src = ( uint8_t* )pcGetLUAError();
    len = strlen( pcGetLUAError() );
  }
  return uDATAget( adr, size, out, src, len );
}

uint8_t uDATAgetSystem ( uint8_t adr, uint8_t size, uint8_t* out )
{
  return uDATAget( adr, size, out, ( uint8_t* )&systemData, sizeof( PDM_DATA ) );
}
