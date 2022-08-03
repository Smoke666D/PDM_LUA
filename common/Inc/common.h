#ifndef COMMON_H
#define COMMON_H
/*----------------------- Includes -------------------------------------*/
#include "stdint.h"
#include "stm32f405xx.h"
/*------------------------ Define --------------------------------------*/
#define  UNIQUE_ID_LENGTH  6U /* in uint16_t */
/*------------------------ Macros --------------------------------------*/
#define  REVERSE_BYTE( b ) ( ( ( b << 7U ) & 0x80U ) | ( ( b << 5U ) & 0x40U ) | ( ( b << 3U ) & 0x20U ) | ( ( b << 1U ) & 0x10U ) | ( ( b >> 1U ) & 0x08U ) | ( ( b >> 3U ) & 0x04U ) | ( ( b >> 5U ) & 0x02U ) | ( ( b >> 7U ) & 0x01U ) )
/*----------------------- Structures -----------------------------------*/
typedef struct
{
  uint16_t      pin;
  GPIO_TypeDef* port;
} PIN_TYPE;
/*----------------------- Functions ------------------------------------*/
void vSYSgetUniqueID32 ( uint32_t* id );
void vSYSgetUniqueID16 ( uint16_t* id );
/*----------------------------------------------------------------------*/
#endif
