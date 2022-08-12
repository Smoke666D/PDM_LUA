/*
 * test.c
 *
 *  Created on: 23 мар. 2022 г.
 *      Author: 79110
 */
/*----------------------- Includes ------------------------------------------------------------------*/
#include "cli.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "common.h"
#include "version.h"
#include "lua.h"
#include "flash.h"
#include "usbhid.h"
#include "PDMhardware.h"
#include "pdm_input.h"
#include "luatask.h"
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
  CLI_TARGET_UNIQUE_STR,
  CLI_TARGET_DOUT_STR,
  CLI_TARGET_DIN_STR,
  CLI_TARGET_CURRENT_STR,
  CLI_TARGET_BAT_STR,
  CLI_TARGET_FLASH_STR,
  CLI_TARGET_SCRIPT_STR,
  CLI_TARGET_LUA_STR,
  CLI_TARGET_VUSB_STR,
  CLI_TARGET_USB_STR,
  CLI_TARGET_VOLTAGE_STR,
  CLI_TARGET_LUA_STATUS_STR,
  CLI_TARGET_LUA_ERROR_STR,
  CLI_TARGET_LUA_TIME_STR,
  CLI_TARGET_LUA_ERROR_COUNTER_STR
};

static const char* const luaStateDic[LUA_STATE_SIZE] = {
  LUA_STATE_INIT_STR,
  LUA_STATE_RUN_STR,
  LUA_STATE_ERROR_STR,
  LUA_STATE_STOP_STR,
  LUA_STATE_RESTART_STR
};
/*----------------------- Variables -----------------------------------------------------------------*/
static TEST_TYPE message   = { 0U };
static uint32_t  scriptAdr = 0U;
/*----------------------- Functions -----------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------*/
/*----------------------- PRIVATE -------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/
uint8_t uCLIparsingFields ( const char* str, char** fields )
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
uint8_t uCLIparse ( const char* str, const char* const* dictionary, uint8_t length )
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
void vCLIparseString ( const char* str, TEST_TYPE* message )
{
  char*   filds[CLI_FILDS_NUMBER] = { 0U };
  uint8_t fieldsCounter = 0U;
  message->cmd      = CLI_COMMAND_NO;
  message->target   = CLI_TARGET_NO;
  message->dataFlag = 0U;
  message->out[0U]  = 0U;
  ( void )memset( message->data, 0U, ( CLI_DATA_FILDS_NUMBER * sizeof( uint32_t ) ) );
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
        for ( uint8_t i=0U; i<message->dataFlag; i++ )
        {
          message->data[i] = atoi( filds[CLI_SYSTEM_FILDS_NUMBER + i] );
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
      res = ( uint8_t )strlen( CLI_ERROR_OK_STR ) + 1U;
      break;
    case CLI_STATUS_ERROR_COMMAND:
      ( void )strcpy( buf, CLI_ERROR_COMMAND_STR );
      ( void )strcat( buf, CLI_LINE_END );
      res = strlen( CLI_ERROR_COMMAND_STR ) + 1U;
      break;
    case CLI_STATUS_ERROR_TARGET:
      ( void )strcpy( buf, CLI_ERROR_TARGET_STR );
      ( void )strcat( buf, CLI_LINE_END );
      res = ( uint8_t )strlen( CLI_ERROR_TARGET_STR ) + 1U;
      break;
    case CLI_STATUS_ERROR_DATA:
      ( void )strcpy( buf, CLI_ERROR_DATA_STR );
      ( void )strcat( buf, CLI_LINE_END );
      res = ( uint8_t )strlen( CLI_ERROR_DATA_STR ) + 1U;
      break;
    case CLI_STATUS_ERROR_EXECUTING:
      ( void )strcpy( buf, CLI_ERROR_EXECUTING_STR );
      ( void )strcat( buf, CLI_LINE_END );
      res = ( uint8_t )strlen( CLI_ERROR_EXECUTING_STR ) + 1U;
      break;
    default:
      ( void )strcpy( buf, CLI_ERROR_UNKNOWN );
      ( void )strcat( buf, CLI_LINE_END );
      res = ( uint8_t )strlen( CLI_ERROR_UNKNOWN ) + 1U;
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
    res = ( uint8_t )strlen( CLI_DIO_ON_STR ) + 1U;
  }
  else
  {
    ( void )strcpy( buf, CLI_DIO_OFF_STR);
    ( void )strcat( buf, CLI_LINE_END );
    res = ( uint8_t )strlen( CLI_DIO_OFF_STR ) + 1U;
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
uint8_t uCLIversionToStr ( uint8_t major, uint8_t minor, uint8_t patch, char* buf )
{
  char sub[5U] = { 0U };
  ( void )itoa( ( uint8_t )( major ), sub, 10U );
  ( void )strcat( buf, sub );
  ( void )strcat( buf, "." );
  ( void )itoa( ( uint8_t )( minor ), sub, 10U );
  ( void )strcat( buf, sub );
  ( void )strcat( buf, "." );
  ( void )itoa( ( uint8_t )( patch ), sub, 10U );
  ( void )strcat( buf, sub );
  ( void )strcat( buf, CLI_LINE_END );
  return ( uint8_t )strlen( buf );
}
/*---------------------------------------------------------------------------------------------------*/
uint8_t uCLIserialToStr ( const uint16_t* data, char* buf )
{
  ( void )itoa( ( data[0] | ( ( uint32_t )( data[1U] ) << 16U ) ), buf, 10U );
  return ( uint8_t )strlen( buf );
}
/*---------------------------------------------------------------------------------------------------*/
uint8_t uCLIfloatToStr ( float data, char* out )
{
  ftoa( data, out, 2U );
  ( void )strcat( out, CLI_LINE_END );
  return strlen( out );
}
/*---------------------------------------------------------------------------------------------------*/
CLI_STATUS eCLIprocessSet ( void )
{
  CLI_STATUS res = CLI_STATUS_OK;
  switch ( message.target )
  {
    case CLI_TARGET_DOUT:
      if ( ( message.dataFlag > 0U ) && ( message.data[0U] < OUT_COUNT ) )
      {
        vOutSetState( message.data[0U], 1U );
      }
      else
      {
        res = CLI_STATUS_ERROR_DATA;
      }
      break;
    case CLI_TARGET_FLASH:
      scriptAdr = 0U;
      if ( eFLASHstartWriting() != FLASH_OK )
      {
        res = CLI_STATUS_ERROR_EXECUTING;
      }
      break;
    case CLI_TARGET_SCRIPT:
      if ( message.dataFlag > 0U )
      {
        if ( eFLASHwriteScript( scriptAdr, ( const uint8_t* )( &message.data[0U] ), 1U ) != FLASH_OK )
        {
          res = CLI_STATUS_ERROR_EXECUTING;
        }
        scriptAdr++;
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
  message.length = uCLIstatusToString( res, message.out );
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
CLI_STATUS eCLIprocessReset ( void )
{
  CLI_STATUS res = CLI_STATUS_OK;
  switch ( message.target )
  {
    case CLI_TARGET_DOUT:
      if ( ( message.dataFlag > 0U ) && ( message.data[0U] < OUT_COUNT ) )
      {
        vOutSetState( message.data[0U], 0U );
      }
      else
      {
        res = CLI_STATUS_ERROR_DATA;
      }
      break;
    case CLI_TARGET_FLASH:
      if ( eFLASHendWriting() != FLASH_OK )
      {
        res = CLI_STATUS_ERROR_EXECUTING;
      }
      break;
    default:
      res = CLI_STATUS_ERROR_TARGET;
      break;
  }
  message.length = uCLIstatusToString( res, message.out );
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
CLI_STATUS eCLIprocessGet ( void )
{
  CLI_STATUS res = CLI_STATUS_OK;
  uint16_t   id[UNIQUE_ID_LENGTH] = { 0U };
  uint8_t    length = CLI_MESSAGE_OUT_LENGTH;
  switch ( message.target )
  {
    case CLI_TARGET_DOUT:
      if ( ( message.dataFlag > 0U ) && ( message.data[0U] < OUT_COUNT ) )
      {
        message.length = uCLIdioToStr( eOutGetState( message.data[0U] ), message.out );
      }
      else
      {
        res = CLI_STATUS_ERROR_DATA;
      }
      break;
    case CLI_TARGET_DIN:
      if ( ( message.dataFlag > 0U ) && ( message.data[0U] < DIN_CHANNEL ) )
      {
        message.length = uCLIdioToStr( ucDinGet( message.data[0U] ), message.out );
      }
      else
      {
        res = CLI_STATUS_ERROR_DATA;
      }
      break;
    case CLI_TARGET_BAT:
      res = CLI_STATUS_ERROR_DATA;
      break;
    case CLI_TARGET_CURRENT:
      if ( ( message.dataFlag > 0U ) && ( message.data[0U] < OUT_COUNT ) )
      {
        message.length = uCLIfloatToStr( fOutGetCurrent( message.data[0U] ), message.out );
      }
      else
      {
        res = CLI_STATUS_ERROR_DATA;
      }
      break;
    case CLI_TARGET_VOLTAGE:
      if ( ( message.dataFlag > 0U ) && ( message.data[0U] < AIN_COUNT ) )
      {
        message.length = uCLIfloatToStr( fAinGetState( message.data[0U] ), message.out );
      }
      else
      {
        res = CLI_STATUS_ERROR_DATA;
      }
      break;
    case CLI_TARGET_VUSB:
      message.length = uCLIdioToStr( uUSBisPower(), message.out );
      break;
    case CLI_TARGET_USB:
      message.length = uCLIdioToStr( uUSBisPlug(), message.out );
      break;
    case CLI_TARGET_FLASH:
      message.length = uCLIdioToStr( ( eFLASHgetLockState() == FLASH_LOCKED?1U:0U ), message.out );
      break;
    case CLI_TARGET_LAU:
      res = CLI_STATUS_ERROR_TARGET;
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
            //message.length = uCLIversionToStr( , message.out );
            res = CLI_STATUS_ERROR_DATA;
            break;
          case CLI_VERSION_FIRMWARE:
            message.length = uCLIversionToStr( FIRMWARE_VERSION_MAJOR, FIRMWARE_VERSION_MINOR, FIRMWARE_VERSION_PATCH, message.out );
            break;
          case CLI_VERSION_HARDWARE:
            message.length = uCLIversionToStr( HARDWARE_VERSION_MAJOR, HARDWARE_VERSION_MINOR, HARDWARE_VERSION_PATCH, message.out );
            break;
          case CLI_VERSION_LUA:
            ( void )strcpy( message.out, LUA_VERSION_MAJOR );
            ( void )strcat( message.out, "." );
            ( void )strcat( message.out, LUA_VERSION_MINOR );
            ( void )strcat( message.out, CLI_LINE_END );
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
    case CLI_TARGET_LUA_STATUS:
      strcpy( message.out, luaStateDic[eLUAgetSTATE()] );
      strcat( message.out, CLI_LINE_END );
      message.length = strlen( message.out );
      break;
    case CLI_TARGET_LUA_ERROR:
      if ( CLI_MESSAGE_OUT_LENGTH > strlen( pcLUAgetErrorString() ) )
      {
        length = strlen( pcLUAgetErrorString() );
      }
      memcpy( message.out, pcLUAgetErrorString(), length );
      strcat( message.out, CLI_LINE_END );
      message.length = strlen( message.out );
      break;
    case CLI_TARGET_LUA_TIME:
      itoa( ( ulLUAgetWorkCicle() * 10U ), message.out, 10U );
      strcat( message.out, " ms" );
      strcat( message.out, CLI_LINE_END );
      message.length = strlen( message.out );
      break;
    case CLI_TARGET_LUA_ERROR_COUNTER:
      itoa( ucLUAgetErrorCount(), message.out, 10U );
      strcat( message.out, CLI_LINE_END );
      message.length = strlen( message.out );
      break;
    default:
      res = CLI_STATUS_ERROR_TARGET;
      break;
  }
  if ( res != CLI_STATUS_OK )
  {
    message.length = uCLIstatusToString( res, message.out );
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
/*----------------------- PABLIC --------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/
CLI_STATUS eCLIprocess ( const char* str, uint8_t length )
{
  CLI_STATUS res = CLI_STATUS_OK;
  ( void )memset( message.out, '\0', CLI_MESSAGE_OUT_LENGTH );
  vCLIparseString( str, &message );
  switch ( message.cmd )
  {
    case CLI_COMMAND_SET:
      res = eCLIprocessSet();
      break;
    case CLI_COMMAND_RESET:
      res = eCLIprocessReset();
      break;
    case CLI_COMMAND_GET:
      res = eCLIprocessGet();
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
