/*
 * usbhid.c
 *
 *  Created on: 2 апр. 2020 г.
 *      Author: photo_Mickey
 */
/*----------------------- Includes ------------------------------------------------------------------*/
#include "usbhid.h"
#include "usbd_conf.h"
#include "usb_device.h"
#include "flash.h"
#include "data.h"
#include "datastorage.h"
/*----------------------- Structures ----------------------------------------------------------------*/
extern RTC_HandleTypeDef hrtc;
extern USBD_HandleTypeDef  hUsbDeviceFS;
/*----------------------- Constant ------------------------------------------------------------------*/
/*----------------------- Variables -----------------------------------------------------------------*/
static uint8_t      usbPlug                       = 0U;
static uint8_t      outputBuffer[USB_REPORT_SIZE] = { 0U };
static uint8_t      inputBuffer[USB_REPORT_SIZE]  = { 0U };
static osThreadId_t usbHandle                     = NULL;
static PIN_TYPE*    usbPullUpPin                  = NULL;
static PIN_TYPE*    usbDetectorPin                = NULL;
/*----------------------- Functions -----------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/
void vUint32ToBytes ( uint32_t input, uint8_t* output )
{
  output[0U] = ( uint8_t )( ( input ) );
  output[1U] = ( uint8_t )( ( input ) >> 8U );
  output[2U] = ( uint8_t )( ( input ) >> 16U );
  output[3U] = ( uint8_t )( ( input ) >> 24U );
  return;
}
/*---------------------------------------------------------------------------------------------------*/
void vUint24ToBytes ( uint32_t input, uint8_t* output )
{
  output[0U] = ( uint8_t )( ( input ) );
  output[1U] = ( uint8_t )( ( input ) >> 8U );
  output[2U] = ( uint8_t )( ( input ) >> 16U );
  return;
}
/*---------------------------------------------------------------------------------------------------*/
void vUint16ToBytes ( uint16_t input, uint8_t* output )
{
  output[0U] = ( uint8_t )( input );
  output[1U] = ( uint8_t )( input >> 8U );
  return;
}
/*---------------------------------------------------------------------------------------------------*/
uint16_t uByteToUint16 ( const uint8_t* data )
{
  return ( ( uint16_t )data[0U] ) | ( ( ( uint16_t )data[1U] ) << 8U );
}
/*---------------------------------------------------------------------------------------------------*/
uint32_t uByteToUint24 ( const uint8_t* data )
{
  return ( ( ( uint32_t )( data[2U] ) ) << 16U ) |
         ( ( ( uint32_t )( data[1U] ) ) <<  8U ) |
           ( ( uint32_t )( data[0U] ) );
}
/*---------------------------------------------------------------------------------------------------*/
uint32_t uByteToUint32 ( const uint8_t* data )
{
  return ( ( ( uint32_t )( data[3U] ) ) << 24U ) |
         ( ( ( uint32_t )( data[2U] ) ) << 16U ) |
         ( ( ( uint32_t )( data[1U] ) ) <<  8U ) |
           ( ( uint32_t )( data[0U] ) );
}
/*---------------------------------------------------------------------------------------------------*/
/*
 * Get connection status of USB device
 * input:  output
 * output: status
 */
USB_CONN_STATUS eUSBgetStatus ( void )
{
  USB_CONN_STATUS res = USB_DISCONNECT;

  if ( hUsbDeviceFS.dev_state == USBD_STATE_CONFIGURED )
  {
    res = USB_CONNECT;
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
void vUSBinit ( const PIN_TYPE* usbDet, const PIN_TYPE* usbPullup )
{
  vDATAinit();
  usbPullUpPin   = ( PIN_TYPE* )usbPullup;
  usbDetectorPin = ( PIN_TYPE* )usbDet;
  HAL_GPIO_WritePin( usbPullUpPin->port, usbPullUpPin->pin, GPIO_PIN_SET );
  return;
}
/*---------------------------------------------------------------------------------------------------*/
uint8_t uUSBisPower ( void )
{
  uint8_t res = 0U;
  if ( HAL_GPIO_ReadPin( usbDetectorPin->port, usbDetectorPin->pin ) == GPIO_PIN_SET )
  {
    res = 1U;
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
uint8_t uUSBisPlug ( void )
{
  return usbPlug;
}
/*---------------------------------------------------------------------------------------------------*/
/*
 * Send data via USB
 * input:  array of data
 * output: status of transmission
 */
USBD_StatusTypeDef eUSBwrite ( uint8_t* data, uint8_t length )
{
  return USBD_CUSTOM_HID_SendReport( &hUsbDeviceFS, data, USB_REPORT_SIZE );
}
/*---------------------------------------------------------------------------------------------------*/
/*
 * Convert report structure to the output buffer
 * input:  report structure
 * output: none
 */
void vUSBmakeReport ( USB_REPORT* report )
{
  uint8_t len = USB_DATA_SIZE;
  report->buf[USB_DIR_BYTE]  = USB_GET_DIR_VAL( report->dir );
  report->buf[USB_CMD_BYTE]  = report->cmd;
  report->buf[USB_STAT_BYTE] = report->stat;
  report->buf[USB_LEN0_BYTE] = report->length;
  vUint16ToBytes( report->adr, &report->buf[USB_ADR0_BYTE] );

  if ( report->length < USB_DATA_SIZE )
  {
    len = report->length;
  }
  for ( uint8_t i=0U; i<len; i++ )
  {
    report->buf[USB_DATA_BYTE + i] = report->data[i];
  }
  return;
}
/*---------------------------------------------------------------------------------------------------*/
/*
 * Parse input buffer to the report structure
 * input:  report structure
 * output: none
 */
void vUSBparseReport ( USB_REPORT* report )
{
  report->dir    = USB_GET_DIR( report->buf[USB_DIR_BYTE] );
  report->cmd    = ( USB_REPORT_CMD )report->buf[USB_CMD_BYTE];
  report->stat   = ( USB_REPORT_STATE )report->buf[USB_STAT_BYTE];
  report->adr    = uByteToUint16( &report->buf[USB_ADR0_BYTE] );
  report->length = report->buf[USB_LEN0_BYTE];
  report->data   = &( report->buf[USB_DATA_BYTE] );
  return;
}
/*---------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/
void vUSBscriptToReport ( USB_REPORT* report )
{
  if ( eFLASHreadScript( report->adr, report->data, report->length ) == FLASH_OK )
  {
    report->stat   = USB_REPORT_STATE_OK;
    report->length = USB_DATA_SIZE;
  }
  else
  {
    report->stat   = USB_REPORT_STATE_BAD_REQ;
    report->length = 0U;
  }
  return;
}
/*---------------------------------------------------------------------------------------------------*/
void eUSBdataToReport ( USB_REPORT* report )
{
  if ( report->adr < ulDATAgetSystemLength() )
  {
    report->stat   = USB_REPORT_STATE_OK;
    report->length = uDATAgetSystem( report->adr, ( USB_DATA_SIZE - 1U ), report->data );
  }
  else
  {
    report->stat   = USB_REPORT_STATE_BAD_REQ;
    report->length = 0U;
  }
  return;
}
void eUSBtelemetryToReport ( USB_REPORT* report )
{
  if ( report->adr < ulDATAgetTelemetryLength() )
  {
    report->stat   = USB_REPORT_STATE_OK;
    report->length = uDATAgetTelemetry( report->adr, ( USB_DATA_SIZE - 1U ), report->data );
  }
  else
  {
    report->stat   = USB_REPORT_STATE_BAD_REQ;
    report->length = 0U;
  }
  return;
}
void eUSBerrorStringToReport ( USB_REPORT* report )
{
  if ( report->adr < ERROR_STRING_LENGTH )
  {
    report->stat   = USB_REPORT_STATE_OK;
    report->length = uDATAgetErrorString( report->adr, ( USB_DATA_SIZE - 1U ), report->data );
  }
  else
  {
    report->stat   = USB_REPORT_STATE_BAD_REQ;
    report->length = 0U;
  }
  return;
}

void vUSBEEPROMToReport  ( USB_REPORT* report )
{
	if ( report->adr  < usGetEEPROMSize() )
	  {
	    report->stat   = USB_REPORT_STATE_OK;
	    report->length = usEEPROMReadToUSB( report->adr, report->data ,  ( USB_DATA_SIZE - 1U ));
	  }
	  else
	  {
	    report->stat   = USB_REPORT_STATE_BAD_REQ;
	    report->length = 0U;
	  }
	  return;
}

void vUSBEEPROMSizeToReport ( USB_REPORT* report )
{
	uint16_t buffer_data = usGetEEPROMSize();
	report->stat   = USB_REPORT_STATE_OK;
	report->length = 3;
	report->data[0] = (buffer_data!=0) ? 1 : 0;
	report->data[1] = buffer_data >> 8;
    report->data[2] = buffer_data & 0xFF;
}


void vUSBTimeToReport ( USB_REPORT* report )
{
	RTC_TimeTypeDef time_buffer;
	RTC_DateTypeDef date_buffer;
	HAL_RTC_GetTime(&hrtc, &time_buffer,  RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &date_buffer, RTC_FORMAT_BIN);
	report->stat   = USB_REPORT_STATE_OK;
	report->length = 6;
	report->data[0] = time_buffer.Hours;
	report->data[1] = time_buffer.Minutes;
	report->data[2] = time_buffer.Seconds;
	report->data[3] = date_buffer.Date;
	report->data[4] = date_buffer.Month;
	report->data[5] = date_buffer.Year;
}

USB_STATUS  eUSBreportToToken( USB_REPORT* report )
{
	USB_STATUS res = USB_STATUS_DONE;
	if (report->length !=2 )
	{
		res = USB_STATUS_ERROR_LENGTH;
	}
	else
	{
		eAccessToken( (uint16_t)( ( report->data[0] << 8) | (report->data[1]) ));
	}
	return (res);
}

USB_STATUS   eUSBStartEEPROMread( USB_REPORT* report )
{
	USB_STATUS res = USB_STATUS_DONE;
	if (iReadEEPROM() == 0)
	{
		res = USB_STATUS_FORBIDDEN;
	}

	return (res);
}


USB_STATUS  eUSBEEPROMwrite( USB_REPORT* report )
{
	USB_STATUS res = USB_STATUS_DONE;
	if (iWriteEEPROM() == 0)
	{
		res = USB_STATUS_FORBIDDEN;
	}

	return (res);
}
USB_STATUS  eUSBreportToTime  ( USB_REPORT* report )
{
    USB_STATUS res = USB_STATUS_DONE;
	RTC_TimeTypeDef time_buffer;
	RTC_DateTypeDef date_buffer;
	if (report->length != 6)
	{
		res = USB_STATUS_ERROR_LENGTH;
	}
	else
	{
		time_buffer.Hours    = report->data[0];
		time_buffer.Minutes  = report->data[1];
		time_buffer.Seconds	 = report->data[2];
		date_buffer.Date	 = report->data[3];
		date_buffer.Month    = report->data[4];
		date_buffer.Year     = report->data[5];
		HAL_RTC_SetTime(&hrtc,&time_buffer,  RTC_FORMAT_BIN);
		HAL_RTC_SetDate(&hrtc, &date_buffer, RTC_FORMAT_BIN);

	}
	return res;
}
/*---------------------------------------------------------------------------------------------------*/
USB_STATUS eUSBreportToScript ( const USB_REPORT* report )
{
  USB_STATUS res = USB_STATUS_DONE;
  switch ( eFLASHwriteScript( report->adr, report->data, report->length ) )
  {
    case FLASH_OK:
      res = USB_STATUS_DONE;
      break;
    case FLASH_ERROR_ADR:
      res = USB_STATUS_ERROR_ADR;
      break;
    case FLASH_ERROR_LENGTH:
      res = USB_STATUS_ERROR_LENGTH;
      break;
    default:
      res = USB_STATUS_STORAGE_ERROR;
      break;
  }
  return res;
}

USB_STATUS eUSBReportToEEPROM( const USB_REPORT* report )
{
	USB_STATUS res = USB_STATUS_DONE;
	  switch ( eEEPROMWriteData( report->adr, report->data, report->length ) )
	  {
	    case FLASH_OK:
	      res = EEPROM_OK;
	      break;
	    case EEPROM_NOT_VALIDE_ADRESS:
	      res = USB_STATUS_ERROR_ADR;
	      break;
	    default:
	      res = USB_STATUS_STORAGE_ERROR;
	      break;
	  }
	  return res;
}

/*---------------------------------------------------------------------------------------------------*/
USB_STATUS eUSBstartWriting ( const USB_REPORT* report )
{
  USB_STATUS res = USB_STATUS_DONE;
  if ( eFLASHstartWriting() != FLASH_OK )
  {
    res = USB_STATUS_STORAGE_ERROR;
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
USB_STATUS eUSBendWriting ( const USB_REPORT* report )
{
  USB_STATUS res = USB_STATUS_DONE;
  if ( eFLASHendWriting() != FLASH_OK )
  {
    res = USB_STATUS_STORAGE_ERROR;
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
USB_STATUS eUSBupdateTelemetry ( const USB_REPORT* report )
{
  USB_STATUS res = USB_STATUS_DONE;
  vDATAupdate();
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
USB_STATUS eUSBrestartLua ( const USB_REPORT* report )
{
  USB_STATUS res = USB_STATUS_DONE;
  vLUArestartPDM();
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/
void vUSBsendReport ( USB_REPORT* report )
{
  vUSBmakeReport( report );
  while ( eUSBwrite( report->buf, ( USB_DATA_BYTE + report->length ) ) == USBD_BUSY )
  {
    osDelay( 2U );
  }
  return;
}
/*---------------------------------------------------------------------------------------------------*/
void vUSBsend ( USB_REPORT* request, void ( *callback )( USB_REPORT* ) )
{
  USB_REPORT report =
  {
    .dir  = USB_REPORT_DIR_OUTPUT,
    .cmd  = request->cmd,
    .adr  = request->adr,
    .buf  = outputBuffer,
    .data = &outputBuffer[USB_DATA_BYTE],
  };
  callback( &report );
  vUSBmakeReport( &report );
  vUSBsendReport( &report );
  return;
}
/*---------------------------------------------------------------------------------------------------*/
void vUSBget ( USB_REPORT* report, USB_STATUS ( *callback )( const USB_REPORT* ) )
{
  USB_REPORT response =
  {
    .cmd    = report->cmd,
    .stat   = USB_REPORT_STATE_OK,
    .adr    = report->adr,
    .length = 0U,
    .buf    = outputBuffer,
    .data   = &outputBuffer[USB_DATA_BYTE],
  };
  switch ( callback( report ) )
  {
    case USB_STATUS_DONE:
      response.stat = USB_REPORT_STATE_OK;
      break;
    case USB_STATUS_CONT:
      response.stat = USB_REPORT_STATE_OK;
      break;
    case USB_STATUS_ERROR_LENGTH:
      response.stat = USB_REPORT_STATE_BAD_REQ;
      break;
    case USB_STATUS_ERROR_ADR:
      response.stat = USB_REPORT_STATE_NON_CON;
      break;
    case USB_STATUS_STORAGE_ERROR:
      response.stat = USB_REPORT_STATE_INTERNAL;
      break;
    default:
      response.stat = USB_REPORT_STATE_BAD_REQ;
      break;
  }
  for ( uint16_t i=0U; i<USB_REPORT_SIZE; i++ )
  {
    outputBuffer[i] = 0U;
  }
  vUSBsendReport( &response );
  return;
}
/*---------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/
/*
 * Handler of USB input interrupt
 * input:  none
 * output: none
 */
void vUSBreceiveHandler ( void )
{
  USBD_CUSTOM_HID_HandleTypeDef* hhid  = ( USBD_CUSTOM_HID_HandleTypeDef* )hUsbDeviceFS.pClassData;
  BaseType_t                     yield = pdFALSE;
  /* Copy input buffer to local buffer */
  for ( uint8_t i=0U; i<USB_REPORT_SIZE; i++ )
  {
    inputBuffer[i] = hhid->Report_buf[i];
  }
  /* Start USB processing task */
  if ( usbHandle != NULL )
  {
    vTaskNotifyGiveFromISR( usbHandle, &yield );
    portYIELD_FROM_ISR( yield );
  }
  return;
}
/*---------------------------------------------------------------------------------------------------*/
void vUSBplugHandler ( void )
{
  usbPlug = 1U;
  return;
}
/*---------------------------------------------------------------------------------------------------*/
void vUSBunplugHandler ( void )
{
  usbPlug = 0U;
  if ( eFLASHgetLockState() == FLASH_UNLOCKED )
  {
    ( void )eFLASHendWriting();
  }
  return;
}
/*---------------------------------------------------------------------------------------------------*/
void vUSBtask ( void *argument )
{
  USB_REPORT report = {.buf = inputBuffer};
  for(;;)
  {
    if ( ulTaskNotifyTake( pdTRUE, portMAX_DELAY ) > 0U )
    {
      WDT_Reset();
      vUSBparseReport( &report );
      switch( report.cmd )
      {
        /*----------------------------------------*/

      	  case USB_REPORT_CMD_GET_EEPROM:
      		vUSBsend( &report, vUSBEEPROMToReport );
      		 break;
      	  case USB_REPORT_CMD_GET_EEPROM_SIZE:
            vUSBsend( &report, vUSBEEPROMSizeToReport );
            break;
        case USB_REPORT_CMD_GET_TIME_DATE:
        	vUSBsend( &report, vUSBTimeToReport );
           break;
        case USB_REPORT_CMD_READ_SCRIPT:
          vUSBsend( &report, vUSBscriptToReport );
          break;
        case USB_REPORT_CMD_READ_DATA:
          vUSBsend( &report, eUSBdataToReport );
          break;
        case USB_REPORT_CMD_READ_TELEMETRY:
          vUSBsend( &report, eUSBtelemetryToReport );
          break;
        case USB_REPORT_CMD_READ_ERROR_STR:
          vUSBsend( &report, eUSBerrorStringToReport );
          break;
        /*----------------------------------------*/
        case USB_REPORT_CMD_END_EEPROM_WRITE:
        	vUSBget( &report, eUSBEEPROMwrite);
        	break;
        case USB_REPORT_CMD_SET_EEPROM:
        	vUSBget( &report, eUSBReportToEEPROM);
        	break;
        case USB_REPORT_CMD_START_EEPROM_READ:
        	vUSBget( &report, eUSBStartEEPROMread );
        	break;
        case USB_REPORT_CMD_SEND_ACCESS_TOKEN:
        	vUSBget( &report, eUSBreportToToken );
        	break;
        case USB_REPORT_CMD_SET_TIME_DATE:
          vUSBget( &report, eUSBreportToTime );
          break;
        case USB_REPORT_CMD_WRITE_SCRIPT:
          vUSBget( &report, eUSBreportToScript );
          break;
        case USB_REPORT_CMD_UPDATE_TELEMETRY:
          vUSBget( &report, eUSBupdateTelemetry );
          break;
        case USB_REPORT_CMD_START_WRITING:
          vUSBget( &report, eUSBstartWriting );
          break;
        case USB_REPORT_CMD_END_WRITING:
          vUSBget( &report, eUSBendWriting );
          break;
        case USB_REPORT_CMD_RESTART_LUA:
          vUSBget( &report, eUSBrestartLua );
          break;
        /*----------------------------------------*/
        default:
          break;
      }
    }
  }
  return;
}
/*---------------------------------------------------------------------------------------------------*/
osThreadId_t* osUSBgetTaskHandle ( void )
{
  return &usbHandle;
}
/*---------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/
