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
#include "PDMhardware.h"
#include "pdm_input.h"
#include "luatask.h"

uint8_t uDATAfloatToByte ( float input, uint8_t* out )
{
  *( uint32_t* )out = *( uint32_t* )&input;
  return sizeof( float );
}
uint8_t uDATAgetArray ( uint8_t size, uint8_t* out, uint8_t ( *get )( uint8_t adr ) )
{
  uint8_t length  = ceil( size / 8U );
  ( void )memset( out, 0U, length );
  for ( uint8_t i=0U; i<size; i++ )
  {
    uint8_t byte = ( uint8_t )( i / 8U );
    out[byte] |= ( get( i ) & 0x01U ) << ( ( uint8_t )( i - ( byte * 8U ) ) );
  }
  return length;
}
uint8_t uDATAdoutToBytes ( OUT_NAME_TYPE n, uint8_t* out )
{
  *( uint32_t* )out = ( uint32_t )eOutGetState( n );
  return sizeof( PDM_OUT_STATE_t );
}
DATA_ERROR eDATAget ( DATA_ADR adr, uint8_t* out, uint8_t* length, uint8_t size )
{
  DATA_ERROR res = DATA_OK;
  *length = 0U;
  switch ( adr )
  {
    case DATA_ADR_UNIQUE_0:
      *length = sizeof( uint32_t );
      *( uint32_t* )out = HAL_GetUIDw0();
      break;
    case DATA_ADR_UNIQUE_1:
      *length = sizeof( uint32_t );
      *( uint32_t* )out = HAL_GetUIDw1();
      break;
    case DATA_ADR_UNIQUE_2:
      *length = sizeof( uint32_t );
      *( uint32_t* )out = HAL_GetUIDw2();
      break;
    case DATA_ADR_CURRENT_0:
      *length = uDATAfloatToByte( fOutGetCurrent( OUT_1 ), out );
      break;
    case DATA_ADR_CURRENT_1:
      *length = uDATAfloatToByte( fOutGetCurrent( OUT_2 ), out );
      break;
    case DATA_ADR_CURRENT_2:
      *length = uDATAfloatToByte( fOutGetCurrent( OUT_3 ), out );
      break;
    case DATA_ADR_CURRENT_3:
      *length = uDATAfloatToByte( fOutGetCurrent( OUT_4 ), out );
      break;
    case DATA_ADR_CURRENT_4:
      *length = uDATAfloatToByte( fOutGetCurrent( OUT_5 ), out );
      break;
    case DATA_ADR_CURRENT_5:
      *length = uDATAfloatToByte( fOutGetCurrent( OUT_6 ), out );
      break;
    case DATA_ADR_CURRENT_6:
      *length = uDATAfloatToByte( fOutGetCurrent( OUT_7 ), out );
      break;
    case DATA_ADR_CURRENT_7:
      *length = uDATAfloatToByte( fOutGetCurrent( OUT_8 ), out );
      break;
    case DATA_ADR_CURRENT_8:
      *length = uDATAfloatToByte( fOutGetCurrent( OUT_9 ), out );
      break;
    case DATA_ADR_CURRENT_9:
      *length = uDATAfloatToByte( fOutGetCurrent( OUT_10 ), out );
      break;
    case DATA_ADR_CURRENT_10:
      *length = uDATAfloatToByte( fOutGetCurrent( OUT_11 ), out );
      break;
    case DATA_ADR_CURRENT_11:
      *length = uDATAfloatToByte( fOutGetCurrent( OUT_12 ), out );
      break;
    case DATA_ADR_CURRENT_12:
      *length = uDATAfloatToByte( fOutGetCurrent( OUT_13 ), out );
      break;
    case DATA_ADR_CURRENT_13:
      *length = uDATAfloatToByte( fOutGetCurrent( OUT_14 ), out );
      break;
    case DATA_ADR_CURRENT_14:
      *length = uDATAfloatToByte( fOutGetCurrent( OUT_15 ), out );
      break;
    case DATA_ADR_CURRENT_15:
      *length = uDATAfloatToByte( fOutGetCurrent( OUT_16 ), out );
      break;
    case DATA_ADR_CURRENT_16:
      *length = uDATAfloatToByte( fOutGetCurrent( OUT_17 ), out );
      break;
    case DATA_ADR_CURRENT_17:
      *length = uDATAfloatToByte( fOutGetCurrent( OUT_18 ), out );
      break;
    case DATA_ADR_CURRENT_18:
      *length = uDATAfloatToByte( fOutGetCurrent( OUT_19 ), out );
      break;
    case DATA_ADR_CURRENT_19:
      *length = uDATAfloatToByte( fOutGetCurrent( OUT_20 ), out );
      break;
    case DATA_ADR_VOLTAGE_BAT:
      *length = uDATAfloatToByte( 0.0, out );
      break;
    case DATA_ADR_VOLTAGE_0:
      *length = uDATAfloatToByte( fAinGetState( AIN_1 ), out );;
      break;
    case DATA_ADR_VOLTAGE_1:
      *length = uDATAfloatToByte( fAinGetState( AIN_2 ), out );;
      break;
    case DATA_ADR_VOLTAGE_2:
      *length = uDATAfloatToByte( fAinGetState( AIN_3 ), out );;
      break;
    case DATA_ADR_VOLTAGE_3:
      *length = uDATAfloatToByte( fAinGetState( AIN_4 ), out );;
      break;
    case DATA_ADR_DIN:
      *length = uDATAgetArray( DIN_CHANNEL, out, ucDinGet );
      break;
    case DATA_ADR_DOUT_0:
      *length = uDATAdoutToBytes( OUT_1, out );
      break;
    case DATA_ADR_DOUT_1:
      *length = uDATAdoutToBytes( OUT_2, out );
      break;
    case DATA_ADR_DOUT_2:
      *length = uDATAdoutToBytes( OUT_3, out );
      break;
    case DATA_ADR_DOUT_3:
      *length = uDATAdoutToBytes( OUT_4, out );
      break;
    case DATA_ADR_DOUT_4:
      *length = uDATAdoutToBytes( OUT_5, out );
      break;
    case DATA_ADR_DOUT_5:
      *length = uDATAdoutToBytes( OUT_6, out );
      break;
    case DATA_ADR_DOUT_6:
      *length = uDATAdoutToBytes( OUT_7, out );
      break;
    case DATA_ADR_DOUT_7:
      *length = uDATAdoutToBytes( OUT_8, out );
      break;
    case DATA_ADR_DOUT_8:
      *length = uDATAdoutToBytes( OUT_9, out );
      break;
    case DATA_ADR_DOUT_9:
      *length = uDATAdoutToBytes( OUT_10, out );
      break;
    case DATA_ADR_DOUT_10:
      *length = uDATAdoutToBytes( OUT_11, out );
      break;
    case DATA_ADR_DOUT_11:
      *length = uDATAdoutToBytes( OUT_12, out );
      break;
    case DATA_ADR_DOUT_12:
      *length = uDATAdoutToBytes( OUT_13, out );
      break;
    case DATA_ADR_DOUT_13:
      *length = uDATAdoutToBytes( OUT_14, out );
      break;
    case DATA_ADR_DOUT_14:
      *length = uDATAdoutToBytes( OUT_15, out );
      break;
    case DATA_ADR_DOUT_15:
      *length = uDATAdoutToBytes( OUT_16, out );
      break;
    case DATA_ADR_DOUT_16:
      *length = uDATAdoutToBytes( OUT_17, out );
      break;
    case DATA_ADR_DOUT_17:
      *length = uDATAdoutToBytes( OUT_18, out );
      break;
    case DATA_ADR_DOUT_18:
      *length = uDATAdoutToBytes( OUT_19, out );
      break;
    case DATA_ADR_DOUT_19:
      *length = uDATAdoutToBytes( OUT_20, out );
      break;
    case DATA_ADR_LUA_STATUS:
      out[0U] = eLUAgetSTATE();
      *length = 1U;
      break;
    case DATA_ADR_LUA_ERROR:
      out[0U] = 0U;
      *length = 1U;
      break;
    case DATA_ADR_LUA_TIME:
      *( uint32_t* )out = ulLUAgetWorkCicle();
      *length = sizeof( uint32_t );
      break;
    case DATA_ADR_LUA_ERROR_COUNTER:
      out[0U] = ucLUAgetErrorCount();
      *length = 1U;
      break;
    default:
      res = DATA_ERROR_ADR;
      break;
  }
  return res;
}

