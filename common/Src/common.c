#include "common.h"
#include "stdint.h"
#include "stm32f4xx_hal.h"
/*---------------------------------------------------------------------------------------------------*/
void vSYSgetUniqueID32 ( uint32_t* id )
{
  id[0U] = HAL_GetUIDw0();
  id[1U] = HAL_GetUIDw1();
  id[2U] = HAL_GetUIDw2();
  return;
}
/*---------------------------------------------------------------------------------------------------*/
void vSYSgetUniqueID16 ( uint16_t* id )
{
  id[0U] = ( uint16_t )( HAL_GetUIDw0() & 0xFFFF );
  id[1U] = ( uint16_t )( ( HAL_GetUIDw0() >> 16U ) & 0xFFFF );
  id[2U] = ( uint16_t )( HAL_GetUIDw1() & 0xFFFF );
  id[3U] = ( uint16_t )( ( HAL_GetUIDw1() >> 16U ) & 0xFFFF );
  id[4U] = ( uint16_t )( HAL_GetUIDw2() & 0xFFFF );
  id[5U] = ( uint16_t )( ( HAL_GetUIDw2() >> 16U ) & 0xFFFF );
  return;
}
/*---------------------------------------------------------------------------------------------------*/
