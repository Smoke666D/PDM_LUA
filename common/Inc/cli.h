/*
 * test.h
 *
 *  Created on: 23 мар. 2022 г.
 *      Author: 79110
 */

#ifndef INC_CLI_H_
#define INC_CLI_H_
/*----------------------- Includes -------------------------------------*/
#include "stm32f4xx_hal.h"
/*------------------------ Define --------------------------------------*/
#define CLI_COMMANDS_NUMBER     3U
#define CLI_TARGETS_NUMBER      10U
#define CLI_MESSAGE_OUT_LENGTH  40U
#define CLI_FIX_DECIMALS        2U
#define CLI_SYSTEM_FILDS_NUMBER 2U
#define CLI_DATA_FILDS_NUMBER   3U
#define CLI_FILDS_NUMBER        ( CLI_SYSTEM_FILDS_NUMBER + CLI_DATA_FILDS_NUMBER )
#define CLI_RELEASED_SIZE       3U
#define CLI_COMMAND_STR_SIZE    5U
#define CLI_TARGET_STR_SIZE     8U

#define CLI_FILD_SEPORATOR      " "
#define CLI_LINE_END            "\n"

#define CLI_SET_COMMAND_STR     "set"
#define CLI_RESET_COMMAND_STR   "reset"
#define CLI_GET_COMMAND_STR     "get"

#define CLI_TARGET_HELP_STR     "help"
#define CLI_TARGET_VERSION_STR  "version"
#define CLI_TARGET_UNIQUE_STR   "unique"
#define CLI_TARGET_DOUT_STR     "dout"
#define CLI_TARGET_DIN_STR      "din"
#define CLI_TARGET_CURRENT_STR  "current"
#define CLI_TARGET_BAT_STR      "bat"
#define CLI_TARGET_FLASH_STR    "flash"
#define CLI_TARGET_SCRIPT_STR   "script"
#define CLI_TARGET_LUA_STR      "lua"


#define CLI_DIO_ON_STR          "on"
#define CLI_DIO_OFF_STR         "off"
#define CLI_ERROR_OK_STR        "Ok"
#define CLI_ERROR_COMMAND_STR   "Wrong command"
#define CLI_ERROR_TARGET_STR    "Wrong target"
#define CLI_ERROR_DATA_STR      "Wrong data"
#define CLI_ERROR_EXECUTING_STR "Executing error"
#define CLI_ERROR_UNKNOWN       "Unknown error"
/*------------------------- Macros -------------------------------------*/
/*-------------------------- ENUM --------------------------------------*/
typedef enum
{
  CLI_VERSION_BOOTLOADER,
  CLI_VERSION_FIRMWARE,
  CLI_VERSION_HARDWARE,
  CLI_VERSION_LUA
} CLI_VERSION;

typedef enum
{
  CLI_STATUS_OK,
  CLI_STATUS_ERROR_COMMAND,
  CLI_STATUS_ERROR_TARGET,
  CLI_STATUS_ERROR_DATA,
  CLI_STATUS_ERROR_EXECUTING
} CLI_STATUS;

typedef enum
{
  CLI_COMMAND_NO,
  CLI_COMMAND_SET,
  CLI_COMMAND_RESET,
  CLI_COMMAND_GET
} CLI_COMMAND;

typedef enum
{
  CLI_TARGET_NO,      /*    00 no  */
  CLI_TARGET_HELP,    /* +- 01 get */
  CLI_TARGET_VERSION, /* ++ 02 get */
  CLI_TARGET_UNIQUE,  /* ++ 03 get */
  CLI_TARGET_DOUT,    /* +  04 get & set */
  CLI_TARGET_DIN,     /* +  05 get */
  CLI_TARGET_CURRENT, /* +  06 get */
  CLI_TARGET_BAT,     /* +  07 get */
  CLI_TARGET_FLASH,   /* ++ 08 get & set */
  CLI_TARGET_SCRIPT,  /* ++ 09 set */
  CLI_TARGET_LAU      /* +  10 get */
} CLI_TARGET;
/*----------------------- Structures -----------------------------------*/
typedef struct
{
  CLI_COMMAND cmd;
  CLI_TARGET  target;
  uint8_t     dataFlag;
  uint8_t     length;
  uint32_t    data[CLI_DATA_FILDS_NUMBER];
  char        out[CLI_MESSAGE_OUT_LENGTH];
} TEST_TYPE;
/*------------------------ Functions -----------------------------------*/
#if defined ( UNIT_TEST )
  uint8_t     uCLIversionToStr ( uint8_t major, uint8_t minor, uint8_t patch, char* buf );
  uint8_t     uCLIparsingFields ( const char* str, char** filds );
  uint8_t     uCLIparse ( const char* str, const char* const* dictionary, uint8_t length );
  void        vCLIparseString ( const char* str, TEST_TYPE* message );
  char*       cCLIparseTimeFild ( char* pStr, uint8_t* output );
  uint8_t     uCLIstatusToString ( CLI_STATUS status, char* buf );
  uint8_t     uCLIdioToStr ( uint8_t state, char* buf );
  uint8_t     uCLIhexToStr ( uint8_t* data, uint8_t length, char* buf );
#endif
CLI_STATUS eCLIprocess ( const char* str, uint8_t length );
char*      cCLIgetOutput ( void );
uint8_t    uCLIgetOutLength ( void );
/*----------------------------------------------------------------------*/
#endif /* INC_CLI_H_ */
