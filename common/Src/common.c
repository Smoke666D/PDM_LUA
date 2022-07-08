#include "common.h"
#include "stdint.h"
/*---------------------------------------------------------------------------------------------------*/
void vSYSgetUniqueID32 ( uint32_t* id )
{
  id[0U] = GET_UNIQUE_ID0;
  id[1U] = GET_UNIQUE_ID1;
  id[2U] = GET_UNIQUE_ID2;
  return;
}
/*---------------------------------------------------------------------------------------------------*/
void vSYSgetUniqueID16 ( uint16_t* id )
{
  id[0U] = ( uint16_t )( GET_UNIQUE_ID0 & 0xFFFF );
  id[1U] = ( uint16_t )( ( GET_UNIQUE_ID0 >> 16U ) & 0xFFFF );
  id[2U] = ( uint16_t )( GET_UNIQUE_ID1 & 0xFFFF );
  id[3U] = ( uint16_t )( ( GET_UNIQUE_ID1 >> 16U ) & 0xFFFF );
  id[4U] = ( uint16_t )( GET_UNIQUE_ID2 & 0xFFFF );
  id[5U] = ( uint16_t )( ( GET_UNIQUE_ID2 >> 16U ) & 0xFFFF );
  return;
}
/*---------------------------------------------------------------------------------------------------*/