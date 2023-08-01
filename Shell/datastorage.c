/*
 * datastorage.c
 *
 *  Created on: 26 окт. 2022 г.
 *      Author: smoke
 */


#include "datastorage.h"
#include "stm32f4xx_hal.h"
#include "system.h"
#include "main.h"


/*
 *  Объявление переменных драйвера
 */
static uint8_t  datacash[EEPROM_SIZE]		    __SECTION(RAM_SECTION_CCMRAM);
static uint8_t  acces_permit					__SECTION(RAM_SECTION_CCMRAM);
static uint8_t  USB_access  = 0;
static EEPROM_DISCRIPTOR DataStorageDiscriptor;



static void eResetDataStorage();
static void vInitDescriptor();
static void vWriteDescriptor();
static void SET_SHORT( EEPROM_ADRESS_TYPE addr, uint16_t data);



void vEEPROMInit(I2C_HandleTypeDef * hi2c2)
{
 eEEPROM( hi2c2);
 acces_permit = 0;
 return;
}


EERPOM_ERROR_CODE_t eIntiDataStorage()
{
	EERPOM_ERROR_CODE_t res = EEPROM_OK;
	res =  eEEPROMRd(VALIDE_CODE_ADDR, datacash, EEPROM_SIZE );

	if ( res== EEPROM_OK )
	{
		if (datacash[VALIDE_CODE_ADDR ] != VALID_CODE)
		{
			eResetDataStorage();
			res =   eEEPROMWr( VALIDE_CODE_ADDR , datacash, EEPROM_SIZE );
			if (res == EEPROM_OK)
			{
				eEEPROMRd(VALIDE_CODE_ADDR, datacash,EEPROM_SIZE);
			}
			else
				eResetDataStorage();
		}
		vInitDescriptor();
	}
	return ( res );
}

/*
 *  Запись регистра с указанием типа данных
 */
EERPOM_ERROR_CODE_t eEEPROMRegTypeWrite( EEPROM_ADRESS_TYPE addr, uint8_t * data, REGISTE_DATA_TYPE_t datatype )
{
	uint8_t Data[REGISTER_SIZE];
	Data[0] =(uint8_t) datatype;
	( void )memcpy(&Data[1],data, REGISTER_SIZE - 1);
	return  ( eEEPROMRegWrite(addr, Data) );
}
/*
 *  Запись абстрактных 5 байт данных в регистр
 */
EERPOM_ERROR_CODE_t eEEPROMRegWrite( EEPROM_ADRESS_TYPE addr, uint8_t * data )
{
	EERPOM_ERROR_CODE_t res = EEPROM_NOT_VALIDE_ADRESS;
	EEPROM_ADRESS_TYPE usAddres = addr * REGISTER_SIZE + REGISTER_OFFSET ;
	if (( addr  <  DataStorageDiscriptor.register_count  ) && (USB_access==0))
	{
		uint8_t Data[REGISTER_SIZE];
		( void )memcpy(Data,data,REGISTER_SIZE);
		if (memcmp(&datacash[usAddres],Data, REGISTER_SIZE)!=0)
		{
			res = eEEPROMWr(usAddres,&Data[0],REGISTER_SIZE);
			if (res == EEPROM_OK)
			{
				( void )memcpy(&datacash[usAddres],data,REGISTER_SIZE);
		    }
		}
	}
	return ( res );
}
/*
 *
 */
EERPOM_ERROR_CODE_t eEEPROMReadRegTpye( EEPROM_ADRESS_TYPE addr, REGISTE_DATA_TYPE_t * data_type )
{
	EEPROM_ADRESS_TYPE usAddres = addr * REGISTER_SIZE  + REGISTER_OFFSET ;
    if (( addr  < DataStorageDiscriptor.register_count  ) && (USB_access==0))
	{
    	*data_type = datacash[usAddres];
		return ( EEPROM_OK ) ;
	}
	return ( EEPROM_NOT_VALIDE_ADRESS );
}
/*
 *
 */
EERPOM_ERROR_CODE_t eEEPROMRegRead( EEPROM_ADRESS_TYPE addr, uint8_t * data )
{
	EEPROM_ADRESS_TYPE usAddres = addr * REGISTER_SIZE + REGISTER_OFFSET ;
	if (( addr  <  DataStorageDiscriptor.register_count  ) && (USB_access==0))
	{
	   ( void )memcpy(data,&datacash[usAddres+1U],EEPROM_DATA_FRAME-1U);
		return ( EEPROM_OK );
	}
	return ( EEPROM_NOT_VALIDE_ADRESS );
}


STORAGE_STATUS eGetRecordsStatus()
{
	return (( DataStorageDiscriptor.max_record_count == 0) ? RECORD_NOT_CONFIG : RECORD_IS_CONFIG);
}


void vEEPROMCheckRecord( uint32_t * data_type, uint8_t * record_szie )
{
	if ( eGetRecordsStatus() == RECORD_IS_CONFIG )
	{
		*data_type = datacash[RECORD_FORMAT_ADDR];
		*data_type |= datacash[RECORD_FORMAT_ADDR+1]<<8;
		*data_type |= datacash[RECORD_FORMAT_ADDR+2]<<16;
		*data_type |= datacash[RECORD_FORMAT_ADDR+3]<<24;
		*record_szie =DataStorageDiscriptor.record_fields_count;
	}
	else
	{
		data_type[0] = 0;
		*record_szie = 0;
	}
}

uint16_t vAddRecordToStorage()
{
	uint16_t current_offset;
	if   ( DataStorageDiscriptor.current_reccord_offset  + datacash[RECORD_SIZE_ADDR] > EEPROM_MAX_ADRRES )
	{
		DataStorageDiscriptor.current_reccord_offset  = DataStorageDiscriptor.recod_start_offset;
		DataStorageDiscriptor.record_pointer = 0;
	}
	else
	{
		DataStorageDiscriptor.record_pointer++;
	}
	if (DataStorageDiscriptor.record_count  < DataStorageDiscriptor.max_record_count )
	{
		DataStorageDiscriptor.record_count++;
	}
	current_offset = DataStorageDiscriptor.current_reccord_offset;
	DataStorageDiscriptor.current_reccord_offset += datacash[RECORD_SIZE_ADDR];
	vWriteDescriptor();
	eEEPROMWr(0, datacash, REGISTER_OFFSET);
	return (current_offset);


}

EERPOM_ERROR_CODE_t eEEPROMAddReg(  uint8_t * data_type )
{
	EERPOM_ERROR_CODE_t res = EEPROM_NOT_VALIDE_ADRESS;
	uint16_t record_start_index = 0;
    if ( ( eGetRecordsStatus() == RECORD_IS_CONFIG ) && (USB_access==0))
    {

    	record_start_index  = vAddRecordToStorage();
    	for (uint8_t i=0;i<datacash[RECORD_SIZE_ADDR];i++)
    	{
    		datacash[record_start_index + i ] = data_type[i];
    	}
    	eEEPROMWr(record_start_index,&datacash[record_start_index],datacash[RECORD_SIZE_ADDR]);
    }
	return ( res );
}






uint16_t usGetEEPROMSize()
{
	return (acces_permit == 1) ? (EEPROM_MAX_ADRRES+1) : 0;
}

int iWriteEEPROM()
{
	if (acces_permit == 1 )
	{
		eEEPROMWr( VALIDE_CODE_ADDR , &datacash[0], EEPROM_MAX_ADRRES );
	}
	return (acces_permit );
}
int iReadEEPROM()
{
	if (acces_permit == 1 )
	{
		eEEPROMRd( VALIDE_CODE_ADDR, &datacash[0], EEPROM_MAX_ADRRES );
	}
	return (acces_permit );
}

int eAccessToken( uint16_t token)
{
	if (GET_SHORT(ACCESS_TOKEN_ADDR) == 0)
	{
		SET_SHORT( ACCESS_TOKEN_ADDR,  token);
		eEEPROMWr(ACCESS_TOKEN_ADDR, &datacash[ACCESS_TOKEN_ADDR],2);
		acces_permit  = 1;
	}
	else
	{
		//if (acces_permit == 1 )
		//{
		//	SET_SHORT( ACCESS_TOKEN_ADDR,  token);
		//	HAL_I2C_Master_Transmit(I2C, Device_ADD | GET_ADDR_MSB( ACCESS_TOKEN_ADDR) , &datacash[ACCESS_TOKEN_ADDR], 2, EEPROM_TIME_OUT );
		//}
		//else
		//{
			acces_permit = (GET_SHORT(ACCESS_TOKEN_ADDR) == token) ? 1 : 0;
		//}
	}
	return  (acces_permit);
}

/*
 * Функци записи данных даты и времени в нужны регистр
 */
void vSetTimeToReg( uint8_t * DataStorage, PDM_DATA_TIME data)
{

 uint32_t date_time;
 date_time = (uint32_t)(data.Second & 0x3F);
 date_time |= (uint32_t)(data.Minute & 0x3F) << 6;
 date_time |= (uint32_t)(data.Hour & 0x1F) << 12;
 date_time |= (uint32_t)((data.Year) & 0x7F) << 17;
 date_time |= (uint32_t)(data.Month & 0xF) << 24;
 date_time |= (uint32_t)(data.Day & 0x0F) << 28;
 DataStorage[0] = (TIME_STAMP  | ((data.Day & 0x10) >> 4));
 DataStorage[1] = (uint8_t)((date_time >> 24) & 0xFF);
 DataStorage[2] = (uint8_t)((date_time >> 16) & 0xFF);
 DataStorage[3] = (uint8_t)((date_time >> 8) & 0xFF);
 DataStorage[4] = (uint8_t)(date_time & 0xFF);
 return;
}
void vGetRegToTime( uint8_t * DataStorage, PDM_DATA_TIME * data)
{
 uint32_t date_time;
 date_time = DataStorage[4] | DataStorage[1]<<8 | DataStorage[2]<<16 | DataStorage[1]<<24;
 data->Second = date_time & 0x3F;
 data->Minute = (date_time >> 6) & 0x3F;
 data->Hour = (date_time >> 12) & 0x1F;
 data->Year = (date_time >> 17) & 0x7F;
 data->Month = (date_time >> 24) & 0x0F;
 data->Day = (date_time >> 28) & 0x0F | (DataStorage[0] & 0x01)<<4;
 return;
}
uint16_t usEEPROMReadToChashToUSB()
{
	if (acces_permit)
	{
		eIntiDataStorage();
	}
	return  ( acces_permit );
}
uint16_t usEEPROMReadToUSB(uint16_t addr, uint8_t * data, uint8_t len )
{
	if (acces_permit)
	{
		USB_access = 1;
		uint8_t* source = &datacash[addr];
		uint16_t remain = usGetEEPROMSize() - addr;
		uint8_t  length = ( remain > len ) ? len : ( uint8_t )remain;
	    ( void )memcpy( data, source, length );
	    USB_access = 0;
		return ( length );
	}
	return ( 0 );
}

EERPOM_ERROR_CODE_t eEEPROMWriteData ( uint32_t adr, const uint8_t* data, uint32_t length )
{
	EERPOM_ERROR_CODE_t     res       = EEPROM_OK;
	if ( acces_permit)
	{
		if ( length + adr  <= EEPROM_MAX_ADRRES )
		{
			USB_access = 1;
			uint8_t* source = &datacash[adr];
			( void )memcpy(  source, data, length );
			USB_access = 0;
        }
		else
		{
			res = EEPROM_NOT_VALIDE_ADRESS;
		}
	}
	else
	{
		res = EEPROM_ACCESS_ERROR;
	}
  return res;
}

uint8_t ucData[17];








/*******************************************************PRIVATE********************************************************************/
/*
 *   функция сборса хранилища
 */
static void eResetDataStorage()
{
	 ( void )memset(datacash,0U,EEPROM_SIZE);
	 datacash[VALIDE_CODE_ADDR ] = VALID_CODE;
	 return;
}

static void vInitDescriptor()
{
	uint8_t offset = 0;
	DataStorageDiscriptor.record_fields_count = 0;
	DataStorageDiscriptor.register_count     = GET_REG(REGISTER_COUNT_ADDR);
	if ( datacash[RECORD_SIZE_ADDR ] !=0 )
	{
		DataStorageDiscriptor.record_pointer     = GET_REG(RECORD_POINTER_ADDR);
		DataStorageDiscriptor.record_count       = GET_REG(RECORD_COUNT_ADDR);
		DataStorageDiscriptor.recod_start_offset = GET_REG(REGISTER_COUNT_ADDR)*REGISTER_SIZE + REGISTER_OFFSET;
		DataStorageDiscriptor.max_record_count   = (EEPROM_MAX_ADRRES - DataStorageDiscriptor.recod_start_offset)/datacash[RECORD_SIZE_ADDR ];
		uint32_t temp_data_format = datacash[RECORD_FORMAT_ADDR] | datacash[RECORD_FORMAT_ADDR+1]<<8 | datacash[RECORD_FORMAT_ADDR+2]<<16 | datacash[RECORD_FORMAT_ADDR+3]<<24;;
		if ((temp_data_format & DATA_TYPE_MASK) == 0 )
		{
			offset = 1;
			DataStorageDiscriptor.record_fields_count = 1;
			temp_data_format>>=2;
		}
		for ( int i = offset; i < MAX_RECORD_SIZE ; i++)
		{
			if ((temp_data_format & DATA_TYPE_MASK )!=0)
			{
				DataStorageDiscriptor.record_fields_count++;
				temp_data_format>>=2;
			}
			else
				break;
		}
		DataStorageDiscriptor.current_reccord_offset = DataStorageDiscriptor.recod_start_offset + datacash[RECORD_SIZE_ADDR ] * GET_REG(RECORD_POINTER_ADDR);
	}
	else
	{
		DataStorageDiscriptor.current_reccord_offset = REGISTER_OFFSET;
		DataStorageDiscriptor.recod_start_offset = 0;
		DataStorageDiscriptor.max_record_count   = 0;
		DataStorageDiscriptor.record_fields_count = 0;
		DataStorageDiscriptor.record_count = 0;
		DataStorageDiscriptor.record_pointer = 0;
	}
}
static void vWriteDescriptor()
{
	SET_SHORT( RECORD_COUNT_ADDR,  DataStorageDiscriptor.record_count);
	SET_SHORT(RECORD_POINTER_ADDR, DataStorageDiscriptor.record_pointer);
}

static void SET_SHORT( EEPROM_ADRESS_TYPE addr, uint16_t data)
{
	datacash[addr] = data >> 8U;
	datacash[addr+1] = data & 0xFF;
}
