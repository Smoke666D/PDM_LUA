/*
 * flash.c
 *
 *  Created on: Jul 20, 2022
 *      Author: 79110
 */

#include "flash.h"
#include "system.h"
#include "luatask.h"

static FLASH_LOCK flashLock = FLASH_LOCKED;

static HAL_StatusTypeDef eFLASHlock ( void )
{
  flashLock = FLASH_LOCKED;
  return HAL_FLASH_Lock();
}
FLASH_STATE eFLASHwriteScript ( uint32_t adr, const uint8_t* data, uint32_t length )
{
  FLASH_STATE       res       = FLASH_OK;
  HAL_StatusTypeDef status    = HAL_OK;
  uint32_t          curentAdr = FLASH_STORAGE_ADR + adr;
  if ( flashLock == FLASH_UNLOCKED )
  {
    if ( adr < FLASH_STORAGE_LENGTH )
    {
      if ( ( length + adr ) < FLASH_STORAGE_LENGTH )
      {
        for ( uint32_t i=0U; i<length; i++ )
        {
          curentAdr = FLASH_STORAGE_ADR + adr + ( i * sizeof( uint8_t ) );
          status    = HAL_FLASH_Program( FLASH_TYPEPROGRAM_BYTE, curentAdr, ( uint64_t )data[i] );
          if ( status == HAL_TIMEOUT )
          {
            res = FLASH_ERROR_WRITING_TIMEOUT;
            ( void )eFLASHlock();
            break;
          }
          else if ( status != HAL_OK )
          {
            res = FLASH_ERROR_WRITING;
            ( void )eFLASHlock();
            break;
          }
          else if ( *( uint8_t* )( curentAdr ) != data[i] )
          {
            res = FLASH_ERROR_VERIFICATION;
            ( void )eFLASHlock();
            break;
          }
          else
          {
          }
        }
      }
      else
      {
        res = FLASH_ERROR_LENGTH;
        ( void )eFLASHlock();
      }
    }
    else
    {
      res = FLASH_ERROR_ADR;
      ( void )eFLASHlock();
    }
  }
  else
  {
    res = FLASH_ERROR_ACCESS;
  }
  return res;
}
FLASH_STATE eFLASHreadScript ( uint32_t adr, uint8_t* data, uint32_t length )
{
  FLASH_STATE res = FLASH_OK;
  if ( adr < FLASH_STORAGE_LENGTH )
  {
    if ( ( adr + length ) < FLASH_STORAGE_LENGTH )
    {
      for ( uint32_t i=0U; i<length; i++ )
      {
        data[i] = *( uint8_t* )( FLASH_STORAGE_ADR + adr + ( i * sizeof( uint8_t ) ) );
      }
    }
    else
    {
      res = FLASH_ERROR_LENGTH;
    }
  }
  else
  {
    res = FLASH_ERROR_ADR;
  }
  return res;
}
FLASH_STATE eFLASHstartWriting ( void )
{
  FLASH_STATE res = FLASH_OK;
  vLUAstopPDM();
  if ( HAL_FLASH_Unlock() == HAL_OK )
  {
    __HAL_FLASH_CLEAR_FLAG( FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_PGSERR | FLASH_FLAG_PGPERR );
    FLASH_Erase_Sector( FLASH_STORAGE_SECTOR, FLASH_STORAGE_VOLTAGE );
    flashLock = FLASH_UNLOCKED;
  }
  else
  {
    res = FLASH_ERROR_UNLOCK;
  }
  return res;
}
FLASH_STATE eFLASHendWriting ( void )
{
  FLASH_STATE res = FLASH_OK;
  if ( eFLASHlock() != HAL_OK )
  {
    res = FLASH_ERROR_LOCK;
  } else {
    vLUArestartPDM();
  }
  return res;
}
FLASH_LOCK  eFLASHgetLockState ( void )
{
  return flashLock;
}
const char* uFLASHgetScript ( void )
{
  return ( const char* )( FLASH_STORAGE_ADR + FLASH_STORAGE_LENGTH_SIZE );
}
uint32_t uFLASHgetLength ( void )
{
  return *( uint32_t* )( FLASH_STORAGE_ADR );
}

