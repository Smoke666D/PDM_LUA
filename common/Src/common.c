#include "common.h"
#include "stdint.h"
#include "stm32f4xx_hal.h"
#include <math.h>
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
  id[0U] = ( uint16_t )( HAL_GetUIDw0() & 0xFFFFU );
  id[1U] = ( uint16_t )( ( HAL_GetUIDw0() >> 16U ) & 0xFFFFU );
  id[2U] = ( uint16_t )( HAL_GetUIDw1() & 0xFFFF );
  id[3U] = ( uint16_t )( ( HAL_GetUIDw1() >> 16U ) & 0xFFFFU );
  id[4U] = ( uint16_t )( HAL_GetUIDw2() & 0xFFFFU );
  id[5U] = ( uint16_t )( ( HAL_GetUIDw2() >> 16U ) & 0xFFFFU );
  return;
}
/*---------------------------------------------------------------------------------------------------*/
void reverse ( char *str, int len )
{
  int i = 0U;
  int j = len - 1U;
  int temp = 0U;
  while ( i < j )
  {
    temp   = str[i];
    str[i] = str[j];
    str[j] = temp;
    i++;
    j--;
  }
  return;
}
int intToStr ( int x, char str[], int d )
{
  int i = 0U;
  while ( x )
  {
    str[i++] = ( x % 10U ) + '0';
    x = x / 10U;
  }
  while (i < d)
  {
    str[i++] = '0';
  }
  reverse( str, i );
  str[i] = '\0';
  return i;
}
void ftoa ( float n, char* res, int afterpoint )
{
  int   ipart = ( int )n;
  float fpart = n - ( float )ipart;
  int   i     = intToStr( ipart, res, 1U );
  if ( afterpoint != 0U )
  {
    res[i] = '.';
    fpart = fpart * pow( 10U, afterpoint );
    intToStr( ( int )fpart, ( res + i + 1U ), afterpoint );
  }
  return;
}
