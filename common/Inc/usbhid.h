/*
 * usbhid.h
 *
 *  Created on: 2 апр. 2020 г.
 *      Author: photo_Mickey
 */

#ifndef INC_USBHID_H_
#define INC_USBHID_H_
/*----------------------------- Includes -------------------------------------*/
#include "stm32f4xx_hal.h"
#include "common.h"
#include "usbd_customhid.h"
#include "usbd_custom_hid_if.h"
#include "usbd_def.h"
#include "cmsis_os.h"
#include "FreeRTOS.h"
#include "task.h"
/*------------------------------ Enum ----------------------------------------*/
typedef enum
{
  USB_DISCONNECT,
  USB_CONNECT,
} USB_CONN_STATUS;

typedef enum
{
  USB_STATUS_DONE,               /* 0 Operation done */
  USB_STATUS_CONT,               /* 1 Message is not end. Wait for new package */
  USB_STATUS_ERROR_LENGTH,       /* 2 Error in declared and received length of data*/
  USB_STATUS_ERROR_ADR,          /* 3 Wrong address */
  USB_STATUS_ERROR_DATA,         /* 4 Wrong data field */
  USB_STATUS_ERROR_COMMAND,      /* 5 Wrong command */
  USB_STATUS_STORAGE_ERROR,      /* 6 Error in storage process */
  USB_STATUS_FORBIDDEN,          /* 7 Access forbidden */
} USB_STATUS;

typedef enum
{
  USB_REPORT_DIR_OUTPUT,
  USB_REPORT_DIR_INPUT,
} USB_REPORT_DIR;

typedef enum
{
  USB_REPORT_STATE_OK           = 0x01U, /* 200 analog - all OK                             */
  USB_REPORT_STATE_BAD_REQ      = 0x02U, /* 400 analog - request is distorted               */
  USB_REPORT_STATE_NON_CON      = 0x03U, /* 404 analog - requesting a nonexistent resource  */
  USB_REPORT_STATE_UNAUTHORIZED = 0x04U, /* 401 analog - unauthorized request               */
  USB_REPORT_STATE_FORBIDDEN    = 0x05U, /* 403 analog - machine not stop, access forbidden */
  USB_REPORT_STATE_INTERNAL     = 0x06U, /* 500 analog - internal server error */
} USB_REPORT_STATE;

typedef enum
{
  USB_REPORT_CMD_NULL,             		/* 0 */
  USB_REPORT_CMD_START_WRITING,    		/* 1 */
  USB_REPORT_CMD_WRITE_SCRIPT,     		/* 2 */
  USB_REPORT_CMD_END_WRITING,      		/* 3 */
  USB_REPORT_CMD_READ_SCRIPT,      		/* 4 */
  USB_REPORT_CMD_READ_DATA,        		/* 5 */
  USB_REPORT_CMD_READ_TELEMETRY,   		/* 6 */
  USB_REPORT_CMD_UPDATE_TELEMETRY, 		/* 7 */
  USB_REPORT_CMD_RESTART_LUA,      		/* 8 */
  USB_REPORT_CMD_READ_ERROR_STR,    	/* 9 */
  USB_REPORT_CMD_GET_TIME_DATE,	  		/* 10 */
  USB_REPORT_CMD_SET_TIME_DATE,			/* 11 */
  USB_REPORT_CMD_SEND_ACCESS_TOKEN,		/* 12 */
  USB_REPORT_CMD_GET_EEPROM_SIZE,		/* 13 */
  USB_REPORT_CMD_GET_EEPROM,			/* 14 */
  USB_REPORT_CMD_SET_EEPROM,       		/* 15 */
  USB_REPORT_CMD_START_EEPROM_READ,     /* 16 */
  USB_REPORT_CMD_END_EEPROM_WRITE,      /* 17 */

} USB_REPORT_CMD;
/*
 * |  0  |  1  |  2   |  3   |  4   |  5   |  6   |  4   |  ...  | ... |  64 |
 * | DIR | CMD | STAT | ADR0 | ADR1 | ADR2 | ADR3 | LEN0 | DATA  | ... | ... |
 */
typedef enum
{
  USB_DIR_BYTE,  /* 0 */
  USB_CMD_BYTE,  /* 1 */
  USB_STAT_BYTE, /* 2 */
  USB_ADR0_BYTE, /* 3 */
  USB_ADR1_BYTE, /* 4 */
  USB_LEN0_BYTE, /* 5 */
  USB_DATA_BYTE, /* 6 */
} USB_BYTES;
typedef enum
{
  USB_INPUT_REPORT_ID  = 0x01U,    /* Direction - first byte in report */
  USB_OUTPUT_REPORT_ID = 0x02U     /* Direction - first byte in report */
} USB_REPORT_ID;
/*------------------------------ Default -------------------------------------*/
#define USB_REPORT_SIZE ( REPORT_COUNT + 1U )
#define USB_DATA_SIZE   ( USB_REPORT_SIZE - USB_DATA_BYTE )
/*------------------------------ Macros --------------------------------------*/
#define USB_GET_DIR_VAL( e ) ( e == USB_REPORT_DIR_INPUT ) ? ( USB_INPUT_REPORT_ID ) : ( USB_OUTPUT_REPORT_ID )
#define USB_GET_DIR( e )     ( e == USB_INPUT_REPORT_ID ) ? ( USB_REPORT_DIR_INPUT ) : ( USB_REPORT_DIR_OUTPUT )
#if ( USBD_CUSTOMHID_OUTREPORT_BUF_SIZE < USB_REPORT_SIZE )
  #error "Wrong USB buffer size"
#endif
/*---------------------------- Structures ------------------------------------*/
typedef struct
{
  USB_REPORT_DIR   dir;     /* Direction of transaction */
  USB_REPORT_CMD   cmd;     /* Command */
  USB_REPORT_STATE stat;    /* Status of transaction */
  uint8_t          length;  /* Length of data array */
  uint16_t         adr;     /* Address of register */
  uint8_t*         data;    /* Data array */
  uint8_t*         buf;     /* Pointer to the IO buffer */
} USB_REPORT;

typedef USB_STATUS ( *dataCallBack )( USB_REPORT* report );  /* callback for data operations */
/*------------------------------ Extern --------------------------------------*/

/*----------------------------- Functions ------------------------------------*/
#if defined( UNIT_TEST )
  void       vUint32ToBytes ( uint32_t input, uint8_t* output );
  void       vUint24ToBytes ( uint32_t input, uint8_t* output );
  void       vUint16ToBytes ( uint16_t input, uint8_t* output );
  uint16_t   uByteToUnit16 ( const uint8_t* data );
  uint32_t   uByteToUint24 ( const uint8_t* data );
  uint32_t   uByteToUint32 ( const uint8_t* data );
  void       vUSBscriptToReport ( USB_REPORT* report );
  USB_STATUS eUSBreportToScript ( const USB_REPORT* report );
  USB_STATUS eUSBstartWriting ( const USB_REPORT* report );
  USB_STATUS eUSBendWriting ( const USB_REPORT* report );
  void       vUSBmakeReport ( USB_REPORT* report );
  void       vUSBparseReport ( USB_REPORT* report );
  USB_STATUS eUSBsaveConfigs ( const USB_REPORT* report );
#endif

void            vUSBinit ( const PIN_TYPE* usbDet, const PIN_TYPE* usbPullup );
uint8_t         uUSBisPower ( void );
uint8_t         uUSBisPlug ( void );
USB_CONN_STATUS eUSBgetStatus ( void );      /* Get connection status of USB device */
void            vUSBreceiveHandler ( void ); /* Handler of USB input interrupt      */
void            vUSBplugHandler ( void );    /* USB plug interrupt                  */
void            vUSBunplugHandler ( void );  /* USB unplug interrupt                */
void            vUSBtask ( void *argument ); /* Processing USB input task           */
osThreadId_t*   osUSBgetTaskHandle ( void ); /* Get USB task handler                */
/*----------------------------------------------------------------------------*/
#endif /* INC_USBHID_H_ */
