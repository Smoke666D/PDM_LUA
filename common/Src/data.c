/*
 * data.c
 *
 *  Created on: Jul 28, 2022
 *      Author: 79110
 */
#include "data.h"

DATA_ERROR eDATAget ( DATA_ADR adr, uint8_t* out, uint8_t* length )
{
  DATA_ERROR res = DATA_OK;
  *length = 0U;
  switch ( adr )
  {
    case DATA_ADR_UNIQUE:
      break;
    case DATA_ADR_CURRENT_0:
      break;
    case DATA_ADR_CURRENT_1:
      break;
    case DATA_ADR_CURRENT_2:
      break;
    case DATA_ADR_CURRENT_3:
      break;
    case DATA_ADR_CURRENT_4:
      break;
    case DATA_ADR_CURRENT_5:
      break;
    case DATA_ADR_CURRENT_6:
      break;
    case DATA_ADR_CURRENT_7:
      break;
    case DATA_ADR_CURRENT_8:
      break;
    case DATA_ADR_CURRENT_9:
      break;
    case DATA_ADR_CURRENT_10:
      break;
    case DATA_ADR_CURRENT_11:
      break;
    case DATA_ADR_CURRENT_12:
      break;
    case DATA_ADR_CURRENT_13:
      break;
    case DATA_ADR_CURRENT_14:
      break;
    case DATA_ADR_CURRENT_15:
      break;
    case DATA_ADR_CURRENT_16:
      break;
    case DATA_ADR_CURRENT_17:
      break;
    case DATA_ADR_CURRENT_18:
      break;
    case DATA_ADR_CURRENT_19:
      break;
    case DATA_ADR_CURRENT_20:
      break;
    case DATA_ADR_VOLTAGE_BAT:
      break;
    case DATA_ADR_VOLTAGE_0:
      break;
    case DATA_ADR_VOLTAGE_1:
      break;
    case DATA_ADR_VOLTAGE_2:
      break;
    case DATA_ADR_VOLTAGE_3:
      break;
    case DATA_ADR_DIN:
      break;
    case DATA_ADR_DOUT:
      break;
    default:
      res = DATA_ERROR_ADR;
      break;
  }
  return res;
}

