/*
 * data.c
 *
 *  Created on: Jul 28, 2022
 *      Author: 79110
 */
#include "data.h"
#include <string.h>
#include "common.h"
#include "stm32f4xx_hal.h"
#include <math.h>
#include "luatask.h"

static PDM_TELEMETRY telemetry = { 0U };
static PDM_DATA      system    = { 0U };


void vDATAinit ( void )
{
  system.uid[0U]          = HAL_GetUIDw0();
  system.uid[1U]          = HAL_GetUIDw1();
  system.uid[2U]          = HAL_GetUIDw2();
  system.firmware.major   = FIRMWARE_VERSION_MAJOR;
  system.firmware.minor   = FIRMWARE_VERSION_MINOR;
  system.firmware.patch   = FIRMWARE_VERSION_PATCH;
  system.hardware.major   = HARDWARE_VERSION_MAJOR;
  system.hardware.minor   = HARDWARE_VERSION_MINOR;
  system.hardware.patch   = HARDWARE_VERSION_PATCH;
  system.bootloader.major = 0U;
  system.bootloader.minor = 0U;
  system.bootloader.patch = 0U;
  telemetry.battery       = 0.0f;
  return;
}

void vDATAupdate ( void )
{
  for ( uint8_t i=0U; i<AIN_COUNT; i++ )
  {
    telemetry.voltage[i] = fAinGetState( ( AIN_NAME_TYPE )i );
  }
  for ( uint8_t i=0U; i<DIN_CHANNEL; i++ )
  {
    telemetry.din[i] = ucDinGet( ( PDM_INPUT_NAME )i );
  }
  for ( uint8_t i=0U; i<OUT_COUNT; i++ )
  {
    telemetry.douts[i].current = fOutGetCurrent( i );
    telemetry.douts[i].max     = fOutGetMaxCurrent( i );
    telemetry.douts[i].state   = eOutGetState( i );
    telemetry.douts[i].error   = eOutGetError( i );
  }
  telemetry.lua.counter = ucLUAgetErrorCount();
  telemetry.lua.time    = ulLUAgetWorkCicle();
  telemetry.lua.state   = eLUAgetSTATE();
  return;
}

static uint8_t uDATAget ( uint8_t adr, uint8_t size, uint8_t* out, uint8_t* data, uint32_t dataSize )
{
  uint8_t* source = &( data )[adr];
  uint32_t remain = sizeof( dataSize ) - adr;
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

uint8_t uDATAgetTelemetry ( uint8_t adr, uint8_t size, uint8_t* out )
{
  return uDATAget( adr, size, out, ( uint8_t* )&telemetry, sizeof( PDM_TELEMETRY ) );
}

uint8_t uDATAgetSystem ( uint8_t adr, uint8_t size, uint8_t* out )
{
  return uDATAget( adr, size, out, ( uint8_t* )&system, sizeof( PDM_DATA ) );
}

