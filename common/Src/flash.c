/*
 * flash.c
 *
 *  Created on: Jul 20, 2022
 *      Author: 79110
 */

#include "flash.h"

FLASH_STATE eFLASHwriteScript ( const uint8_t* data, uint32_t length )
{
  FLASH_STATE       res    = FLASH_OK;
  HAL_StatusTypeDef status = HAL_OK;
  uint32_t          adr    = 0U;
  if ( length < FLASH_STORAGE_LENGTH )
  {
    HAL_FLASH_Unlock();
    __HAL_FLASH_CLEAR_FLAG( FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_PGSERR | FLASH_FLAG_PGPERR );
    FLASH_Erase_Sector( FLASH_STORAGE_SECTOR, FLASH_STORAGE_VOLTAGE );
    for ( uint32_t i=0U; i<length; i++ )
    {
      adr    = FLASH_STORAGE_ADR + ( i * sizeof( uint8_t ) );
      status = HAL_FLASH_Program( FLASH_TYPEPROGRAM_BYTE, adr, ( uint64_t )data[i] );
      if ( status == HAL_TIMEOUT )
      {
        res = FLASH_ERROR_WRITING_TIMEOUT;
        break;
      }
      else if ( status != HAL_OK )
      {
        res = FLASH_ERROR_WRITING;
        break;
      }
      else if ( *( uint8_t* )( adr ) != data[i] )
      {
        res = FLASH_ERROR_VERIFICATION;
        break;
      }
      else
      {

      }
    }
    HAL_FLASH_Lock();
  }
  else
  {
    res = FLASH_ERROR_LENGTH;
  }
  return res;
}

FLASH_STATE eFLASHreadScript ( uint8_t* data, uint32_t length )
{
  FLASH_STATE res = FLASH_OK;
  if ( length < FLASH_STORAGE_LENGTH )
  {
    for ( uint32_t i=0U; i<length; i++ )
    {
      data[i] = *( uint8_t* )( FLASH_STORAGE_ADR + ( i * sizeof( uint8_t ) ) );
    }
  }
  else
  {
    res = FLASH_ERROR_LENGTH;
  }
  return res;
}

