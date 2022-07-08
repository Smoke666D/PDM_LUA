/*
 * test.c
 *
 *  Created on: 23 мар. 2022 г.
 *      Author: 79110
 */
/*----------------------- Includes ------------------------------------------------------------------*/
#include "cli.h"
#include "string.h"
#include "stdlib.h"
/*------------------------- Define ------------------------------------------------------------------*/
/*----------------------- Structures ----------------------------------------------------------------*/
/*----------------------- Constant ------------------------------------------------------------------*/
static const char* const commandStrings[CLI_COMMANDS_NUMBER] = {
  CLI_SET_COMMAND_STR,
  CLI_RESET_COMMAND_STR,
  CLI_GET_COMMAND_STR
};
static const char* const targetStrings[CLI_TARGETS_NUMBER] = {
  CLI_TARGET_VERSION_STR,
  CLI_TARGET_DOUT_STR,
  CLI_TARGET_CURRENT_STR
};
/*----------------------- Variables -----------------------------------------------------------------*/
static TEST_TYPE message = { 0U };
/*----------------------- Functions -----------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------*/
/*----------------------- PRIVATE -------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/
static uint8_t uCLIparsingFields ( const char* str, char** fields )
{
  uint8_t res = 0U;
  char*   p   = NULL;
  if ( str[0U] != 0U )
  {
    fields[0U] = ( char* )str;
    res++;
    for ( uint8_t i=1U; i<CLI_FILDS_NUMBER; i++ )
    {
      p = strstr( fields[i - 1U], CLI_FILD_SEPORATOR );
      if ( p == NULL )
      {
        break;
      }
      else
      {
        fields[i] = p + 1U;
        res++;
      }
    }
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
static uint8_t uCLIparse ( const char* str, const char* const* dictionary, uint8_t length )
{
  uint8_t res = 0U;
  for ( uint8_t i=0U; i<length; i++ )
  {
    if ( strstr( str, dictionary[i] ) == str )
    {
      res = i + 1U;
      break;
    }
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
static void vCLIparseString ( const char* str, TEST_TYPE* message )
{
  char*   filds[CLI_FILDS_NUMBER] = { 0U };
  uint8_t fieldsCounter = 0U;
  message->cmd      = CLI_COMMAND_NO;
  message->target   = CLI_TARGET_NO;
  message->dataFlag = 0U;
  message->out[0U]  = 0U;
  for ( uint8_t i=0U; i<CLI_DATA_FILDS_NUMBER; i++ )
  {
    message->data[i] = 0U;
  }
  fieldsCounter = uCLIparsingFields( str, filds );
  if ( fieldsCounter > 0U )
  {
    message->dataFlag = fieldsCounter - CLI_SYSTEM_FILDS_NUMBER;
    message->cmd      = ( CLI_COMMAND )( uCLIparse( ( const char* )filds[0U], commandStrings, CLI_COMMANDS_NUMBER ) );
    if ( ( fieldsCounter > 1U ) && ( message->cmd != CLI_COMMAND_NO ) )
    {
      message->target = ( CLI_TARGET )( uCLIparse( ( const char* )filds[1U], targetStrings, CLI_TARGETS_NUMBER ) );
      if ( ( message->dataFlag > 0U ) && ( message->target != CLI_TARGET_NO ) )
      {
        if ( ( message->cmd == CLI_COMMAND_SET ) && ( message->target == CLI_TARGET_TIME ) )
        {
          ( void )strcpy( message->out, filds[CLI_SYSTEM_FILDS_NUMBER] );
        }
        else
        {
          for ( uint8_t i=0U; i<message->dataFlag; i++ )
          {
            message->data[i] = atoi( filds[CLI_SYSTEM_FILDS_NUMBER + i] );
          }
        }
      }
    }
  }
  return;
}
/*---------------------------------------------------------------------------------------------------*/
uint8_t uCLIstatusToString ( CLI_STATUS status, char* buf )
{
  uint8_t res = 0U;
  switch ( status )
  {
    case CLI_STATUS_OK:
      ( void )strcpy( buf, CLI_ERROR_OK_STR );
      ( void )strcat( buf, CLI_LINE_END );
      res = strlen( CLI_ERROR_OK_STR ) + 1U;
      break;
    case CLI_STATUS_ERROR_COMMAND:
      ( void )strcpy( buf, CLI_ERROR_COMMAND_STR );
      ( void )strcat( buf, CLI_LINE_END );
      res = strlen( CLI_ERROR_COMMAND_STR ) + 1U;
      break;
    case CLI_STATUS_ERROR_TARGET:
      ( void )strcpy( buf, CLI_ERROR_TARGET_STR );
      ( void )strcat( buf, CLI_LINE_END );
      res = strlen( CLI_ERROR_TARGET_STR ) + 1U;
      break;
    case CLI_STATUS_ERROR_DATA:
      ( void )strcpy( buf, CLI_ERROR_DATA_STR );
      ( void )strcat( buf, CLI_LINE_END );
      res = strlen( CLI_ERROR_DATA_STR ) + 1U;
      break;
    case CLI_STATUS_ERROR_EXECUTING:
      ( void )strcpy( buf, CLI_ERROR_EXECUTING_STR );
      ( void )strcat( buf, CLI_LINE_END );
      res = strlen( CLI_ERROR_EXECUTING_STR ) + 1U;
      break;
    default:
      ( void )strcpy( buf, CLI_ERROR_UNKNOWN );
      ( void )strcat( buf, CLI_LINE_END );
      res = strlen( CLI_ERROR_UNKNOWN ) + 1U;
      break;
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
uint8_t uCLIdioToStr ( uint8_t state, char* buf )
{
  uint8_t res = 0U;
  if ( state > 0U )
  {
    ( void )strcpy( buf, CLI_DIO_ON_STR );
    ( void )strcat( buf, CLI_LINE_END );
    res = strlen( CLI_DIO_ON_STR ) + 1U;
  }
  else
  {
    ( void )strcpy( buf, CLI_DIO_OFF_STR);
    ( void )strcat( buf, CLI_LINE_END );
    res = strlen( CLI_DIO_OFF_STR ) + 1U;
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
uint8_t uCLIhexToStr ( uint8_t* data, uint8_t length, char* buf )
{
  uint8_t i       = 0U;
  char    sub[3U] = { 0U };
  for ( i=0U; i<length; i++ )
  {
    ( void )itoa( data[i], sub, 16U );
   if ( sub[1U] == 0U )
   {
     sub[1U] = '0';
   }
   ( void )strcat( buf, sub );
   if ( i != ( length - 1U ) )
   {
     ( void )strcat( buf, "-" );
   }
  }
  ( void )strcat( buf, CLI_LINE_END );
  return ( ( length * 2U ) + length );
}
/*---------------------------------------------------------------------------------------------------*/
uint8_t uCLIversionToStr ( const uint16_t* version, char* buf )
{
  char sub[5U] = { 0U };
  ( void )itoa( ( uint8_t )( version[0U] ), sub, 10U );
  ( void )strcat( buf, sub );
  ( void )strcat( buf, "." );
  ( void )itoa( ( uint8_t )( version[1U] ), sub, 10U );
  ( void )strcat( buf, sub );
  ( void )strcat( buf, "." );
  ( void )itoa( ( uint8_t )( version[2U] ), sub, 10U );
  ( void )strcat( buf, sub );
  ( void )strcat( buf, CLI_LINE_END );
  return strlen( buf );
}
/*---------------------------------------------------------------------------------------------------*/
uint8_t uCLIserialToStr ( const uint16_t* data, char* buf )
{
  ( void )itoa( ( data[0] | ( ( uint32_t )( data[1U] ) << 16U ) ), buf, 10U );
  return strlen( buf );
}
/*---------------------------------------------------------------------------------------------------*/
/*----------------------- PABLIC --------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/
CLI_STATUS vCLIprocess ( const char* str, uint8_t length )
{
  CLI_STATUS res  = CLI_STATUS_OK;
  RTC_TIME   time = { 0U };
  uint16_t   id[UNIQUE_ID_LENGTH] = { 0U };
  uint8_t    adr = 0U;
  vCLIparseString( str, &message );
  switch ( message.cmd )
  {
    case CLI_COMMAND_SET:
      switch ( message.target )
      {
        case CLI_TARGET_DOUT:
          res = CLI_STATUS_ERROR_DATA;
          break;
        default:
          res = CLI_STATUS_ERROR_TARGET;
          break;
      }
      message.length = uCLIstatusToString( res, message.out );
      break;
    case CLI_COMMAND_RESET:
      switch ( message.target )
      {
        case CLI_TARGET_DOUT:
          res = CLI_STATUS_ERROR_DATA;
          break;
        default:
          res = CLI_STATUS_ERROR_TARGET;
          break;
      }
      message.length = uCLIstatusToString( res, message.out );
      break;
    case CLI_COMMAND_GET:
      switch ( message.target )
      {
        case CLI_TARGET_DOUT:
          res = CLI_STATUS_ERROR_DATA;
          break;
        case CLI_TARGET_BAT:
          res = CLI_STATUS_ERROR_DATA;
          break;
        case CLI_TARGET_CURRENT:
          res = CLI_STATUS_ERROR_DATA;
          break;
        case CLI_TARGET_UNIQUE:
          vSYSgetUniqueID16( id );
          message.length = uCLIhexToStr( ( uint8_t* )id, ( UNIQUE_ID_LENGTH * 2U ), message.out );
          break;
        case CLI_TARGET_VERSION:
          if ( message.dataFlag > 0U )
          {
            switch ( message.data[0U] )
            {
              case CLI_VERSION_BOOTLOADER:
                message.length = uCLIversionToStr( ( const uint16_t* )( versionController.value ), message.out );
                break;
              case CLI_VERSION_FIRMWARE:
                message.length = uCLIversionToStr( ( const uint16_t* )( versionFirmware.value ),   message.out );
                break;
              case CLI_VERSION_HARDWARE:
                message.length = uCLIversionToStr( ( const uint16_t* )( versionBootloader.value ), message.out );
                break;
              case CLI_VERSION_LUA:
                ( void )memset( message.out, 0U, 8U );
                ( void )strcat( message.out, LUA_VERSION_MAJOR );
                ( void )strcat( message.out, "." );
                ( void )strcat( message.out, LUA_VERSION_MINOR );
                message.length = ( uint8_t )strlen( message.out );
                break;
              default:
                res = CLI_STATUS_ERROR_DATA;
                break;
            }
          }
          else
          {
            res = CLI_STATUS_ERROR_DATA;
          }
          break;
        default:
          res = CLI_STATUS_ERROR_TARGET;
          break;
      }
      if ( res != CLI_STATUS_OK )
      {
        message.length = uCLIstatusToString( res, message.out );
      }
      break;
    default:
      res = CLI_STATUS_ERROR_COMMAND;
      message.length = uCLIstatusToString( res, message.out );
      break;
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
char* cCLIgetOutput ( void )
{
  return message.out;
}
/*---------------------------------------------------------------------------------------------------*/
uint8_t uCLIgetOutLength ( void )
{
  return message.length;
}
/*---------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/
