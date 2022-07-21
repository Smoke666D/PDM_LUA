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
#define  FLASH_STORAGE_ADR     0x080E0000UL
#define  FLASH_STORAGE_LENGTH  ( 128U * 1024U ) /* byte */
#define  FLASH_STORAGE_SECTOR  FLASH_SECTOR_11
#define  FLASH_STORAGE_VOLTAGE VOLTAGE_RANGE_3
/*------------------------- Macros -------------------------------------*/
#if ( ( FLASH_STORAGE_ADR + FLASH_STORAGE_LENGTH - 2U ) >= FLASH_END )
  #error "Too big sector for the script storage"
#endif
/*-------------------------- ENUM --------------------------------------*/
typedef enum
{
  FLASH_OK,
  FLASH_ERROR_LENGTH,
  FLASH_ERROR_WRITING,
  FLASH_ERROR_WRITING_TIMEOUT,
  FLASH_ERROR_VERIFICATION
} FLASH_STATE;
/*----------------------- Structures -----------------------------------*/

/*------------------------ Functions -----------------------------------*/
FLASH_STATE    eFLASHwriteScript ( const uint8_t* data, uint32_t length );
FLASH_STATE    eFLASHreadScript ( uint8_t* data, uint32_t length );
const uint8_t* uFLASHgetScript ( void );
/*----------------------------------------------------------------------*/
#endif /* INC_FLASH_H_  */
