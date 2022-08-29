/*
 * flash.h
 *
 *  Created on: Jul 20, 2022
 *      Author: 79110
 */

#ifndef INC_FLASH_H_
#define INC_FLASH_H_
/*----------------------- Includes -------------------------------------*/
#include "stm32f405xx.h"
#include "stm32f4xx_hal.h"
/*------------------------ Define --------------------------------------*/
#define  FLASH_STORAGE_ADR         0x080E0000UL
#define  FLASH_STORAGE_LENGTH      0x20000U /* byte */
#define  FLASH_STORAGE_SECTOR      FLASH_SECTOR_11
#define  FLASH_STORAGE_VOLTAGE     VOLTAGE_RANGE_3
#define  FLASH_STORAGE_LENGTH_SIZE 4U
/*------------------------- Macros -------------------------------------*/
#if ( ( FLASH_STORAGE_ADR + FLASH_STORAGE_LENGTH - 2U ) >= FLASH_END )
  #error "Too big sector for the script storage"
#endif
/*-------------------------- ENUM --------------------------------------*/
typedef enum
{
  FLASH_OK,
  FLASH_ERROR_ADR,
  FLASH_ERROR_LENGTH,
  FLASH_ERROR_UNLOCK,
  FLASH_ERROR_LOCK,
  FLASH_ERROR_ACCESS,
  FLASH_ERROR_WRITING,
  FLASH_ERROR_WRITING_TIMEOUT,
  FLASH_ERROR_VERIFICATION
} FLASH_STATE;

typedef enum
{
  FLASH_LOCKED,
  FLASH_UNLOCKED
} FLASH_LOCK;
/*----------------------- Structures -----------------------------------*/

/*------------------------ Functions -----------------------------------*/
FLASH_STATE eFLASHwriteScript ( uint32_t adr, const uint8_t* data, uint32_t length );
FLASH_STATE eFLASHreadScript ( uint32_t adr, uint8_t* data, uint32_t length );
FLASH_STATE eFLASHstartWriting ( void );
FLASH_STATE eFLASHendWriting ( void );
FLASH_LOCK  eFLASHgetLockState ( void );
const char* uFLASHgetScript ( void );
uint32_t    uFLASHgetLength ( void );
/*----------------------------------------------------------------------*/
#endif /* INC_FLASH_H_  */
