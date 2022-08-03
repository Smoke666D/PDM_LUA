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

void vDATAfloatToByte ( float input, uint8_t* out )
{
  *( uint32_t* )out = *( uint32_t* )&input;
  return;
}

DATA_ERROR eDATAget ( DATA_ADR adr, uint8_t* out, uint8_t* length )
{
  DATA_ERROR res = DATA_OK;
  float temp = 1.0;
  *length = 0U;
  switch ( adr )
  {
    case DATA_ADR_UNIQUE_0:
      *length = 4U;
      *( uint32_t* )out = HAL_GetUIDw0();
      break;
    case DATA_ADR_UNIQUE_1:
      *length = 4U;
      *( uint32_t* )out = HAL_GetUIDw1();
      break;
    case DATA_ADR_UNIQUE_2:
      *length = 4U;
      *( uint32_t* )out = HAL_GetUIDw2();
      break;
    case DATA_ADR_CURRENT_0:
      *length = 4U;
      vDATAfloatToByte( temp, out );
      break;
    case DATA_ADR_CURRENT_1:
      *length = 4U;
      vDATAfloatToByte( temp, out );
      break;
    case DATA_ADR_CURRENT_2:
      *length = 4U;
      vDATAfloatToByte( temp, out );
      break;
    case DATA_ADR_CURRENT_3:
      *length = 4U;
      vDATAfloatToByte( temp, out );
      break;
    case DATA_ADR_CURRENT_4:
      *length = 4U;
      vDATAfloatToByte( temp, out );
      break;
    case DATA_ADR_CURRENT_5:
      *length = 4U;
      vDATAfloatToByte( temp, out );
      break;
    case DATA_ADR_CURRENT_6:
      *length = 4U;
      vDATAfloatToByte( temp, out );
      break;
    case DATA_ADR_CURRENT_7:
      *length = 4U;
      vDATAfloatToByte( temp, out );
      break;
    case DATA_ADR_CURRENT_8:
      *length = 4U;
      vDATAfloatToByte( temp, out );
      break;
    case DATA_ADR_CURRENT_9:
      *length = 4U;
      vDATAfloatToByte( temp, out );
      break;
    case DATA_ADR_CURRENT_10:
      *length = 4U;
      vDATAfloatToByte( temp, out );
      break;
    case DATA_ADR_CURRENT_11:
      *length = 4U;
      vDATAfloatToByte( temp, out );
      break;
    case DATA_ADR_CURRENT_12:
      *length = 4U;
      vDATAfloatToByte( temp, out );
      break;
    case DATA_ADR_CURRENT_13:
      *length = 4U;
      vDATAfloatToByte( temp, out );
      break;
    case DATA_ADR_CURRENT_14:
      *length = 4U;
      vDATAfloatToByte( temp, out );
      break;
    case DATA_ADR_CURRENT_15:
      *length = 4U;
      vDATAfloatToByte( temp, out );
      break;
    case DATA_ADR_CURRENT_16:
      *length = 4U;
      vDATAfloatToByte( temp, out );
      break;
    case DATA_ADR_CURRENT_17:
      *length = 4U;
      vDATAfloatToByte( temp, out );
      break;
    case DATA_ADR_CURRENT_18:
      *length = 4U;
      vDATAfloatToByte( temp, out );
      break;
    case DATA_ADR_CURRENT_19:
      *length = 4U;
      vDATAfloatToByte( temp, out );
      break;
    case DATA_ADR_CURRENT_20:
      *length = 4U;
      vDATAfloatToByte( temp, out );
      break;
    case DATA_ADR_VOLTAGE_BAT:
      *length = 4U;
      vDATAfloatToByte( temp, out );
      break;
    case DATA_ADR_VOLTAGE_0:
      *length = 4U;
      vDATAfloatToByte( temp, out );
      break;
    case DATA_ADR_VOLTAGE_1:
      *length = 4U;
      vDATAfloatToByte( temp, out );
      break;
    case DATA_ADR_VOLTAGE_2:
      *length = 4U;
      vDATAfloatToByte( temp, out );
      break;
    case DATA_ADR_VOLTAGE_3:
      *length = 4U;
      vDATAfloatToByte( temp, out );
      break;
    case DATA_ADR_DIN:
      *length = 2U;
      ( void )memset( out, 0U, *length );
      break;
    case DATA_ADR_DOUT:
      *length = 3U;
      ( void )memset( out, 0U, *length );
      break;
    case DATA_ADR_STATUS:
      *length = 1U;
      ( void )memset( out, 0U, *length );
      break;
    case DATA_ADR_LUA:
      *length = 1U;
      ( void )memset( out, 0U, *length );
      break;
    default:
      res = DATA_ERROR_ADR;
      break;
  }
  return res;
}

