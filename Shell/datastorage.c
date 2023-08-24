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

#include "string.h"

extern RTC_HandleTypeDef hrtc;


/*
 *  Объявление переменных драйвера
 */
static uint8_t  datacash[REGISTER_OFFSET ]		    __SECTION(RAM_SECTION_CCMRAM);
static uint8_t  USB_access  = 0;
static EEPROM_DISCRIPTOR DataStorageDiscriptor  __SECTION(RAM_SECTION_CCMRAM);
static uint8_t record_data[REGISTER_SIZE*MAX_RECORD_SIZE];

static uint8_t buf[EEPROM_SIZE];

static void eResetDataStorage();
static void vInitDescriptor();
static void vWriteDescriptor();
static void SET_SHORT( EEPROM_ADRESS_TYPE addr, uint16_t data);
static uint16_t vAddRecordToStorage();
static void vSetTimeToReg( uint8_t * DataStorage, void * data);
static void vWriteDescriptorReg( uint8_t addr, EEPROM_ADRESS_TYPE data);
STORAGE_ERROR eWriteNewDescriptor( EEPROM_DISCRIPTOR desc);
/*
 * Функция инициализации
 */

void vEEPROMInit(I2C_HandleTypeDef * hi2c2)
{
 eEEPROM( hi2c2);
 DataStorageDiscriptor.access = ACCESS_DENIED;

 return;
}

/*
 * Функция инициализации хранилища данных
 */
EERPOM_ERROR_CODE_t eIntiDataStorage()
{
	EERPOM_ERROR_CODE_t res = EEPROM_OK;
	( void )memset(datacash,0U,REGISTER_OFFSET );
	res =  eEEPROMRd(VALIDE_CODE_ADDR, datacash, REGISTER_OFFSET  );
	if ( res== EEPROM_OK )
	{
		if (datacash[VALIDE_CODE_ADDR ] != VALID_CODE)
		{
			eResetDataStorage();
		}
		vInitDescriptor();
	}
	return ( res );
}

STORAGE_ERROR eCreateDataStorage(EEPROM_ADRESS_TYPE reg_count, uint8_t * record_format_data, uint8_t record_count)
{
	volatile EEPROM_DISCRIPTOR tempDataStorageDiscriptor;
	if ( ((reg_count * REGISTER_SIZE + REGISTER_OFFSET) < EEPROM_SIZE) && ( record_count <= MAX_RECORD_SIZE ) )
	{
		tempDataStorageDiscriptor.register_count  = reg_count;
		tempDataStorageDiscriptor.record_fields_count = record_count;
		tempDataStorageDiscriptor.record_mask = 0;
		tempDataStorageDiscriptor.record_byte_size = 0;
		for (int i=record_count-1; i >=0 ;i--)
		{
			tempDataStorageDiscriptor.record_mask  = tempDataStorageDiscriptor.record_mask  <<2;
			tempDataStorageDiscriptor.record_mask  |= (record_format_data[i] & RECORD_TYPE_MASK);
			switch (record_format_data[i] & RECORD_TYPE_MASK)
			{
					case RECORD_TIME_STAMP:
					case RECORD_LUA:
									tempDataStorageDiscriptor.record_byte_size +=REGISTER_SIZE;
								   	break;
					case RECORD_BYTE:
									tempDataStorageDiscriptor.record_byte_size +=1;
								   	break;
					case RECORD_SHORT:
									tempDataStorageDiscriptor.record_byte_size +=2;
								   	break;
						default:
							 		break;
			}
		}
		tempDataStorageDiscriptor.record_count = 0;
		tempDataStorageDiscriptor.record_pointer = 0;
	}
	eIntiDataStorage();
	if ( (tempDataStorageDiscriptor.register_count != DataStorageDiscriptor.register_count ) ||
			( tempDataStorageDiscriptor.record_mask  != DataStorageDiscriptor.record_mask ))
		{
			eResetDataStorage();
			tempDataStorageDiscriptor.token = DataStorageDiscriptor.token;
			eWriteNewDescriptor( tempDataStorageDiscriptor);
			eIntiDataStorage();
			return (STORAGE_NEW);
		}
	else
		return  ( STORAGE_OK );
}

STORAGE_ERROR eWriteNewDescriptor( EEPROM_DISCRIPTOR desc)
{
	( void )memset(datacash,0U,REGISTER_OFFSET );
	datacash[VALIDE_CODE_ADDR] = VALID_CODE;
	SET_SHORT( ACCESS_TOKEN_ADDR,  desc.token );
	vWriteDescriptorReg(REGISTER_COUNT_ADDR ,  desc.register_count );
	vWriteDescriptorReg(RECORD_COUNT_ADDR ,    desc.record_count );
	vWriteDescriptorReg(RECORD_POINTER_ADDR  , desc.record_pointer );
	datacash[RECORD_SIZE_ADDR] 				=  desc.record_byte_size;
	datacash[RECORD_FORMAT_ADDR] = desc.record_mask & BYTE_MASK;
	datacash[RECORD_FORMAT_ADDR + SECOND_BYTE_ADDR ] = ( desc.record_mask >> SECOND_BYTE_OFS ) & BYTE_MASK;
	datacash[RECORD_FORMAT_ADDR + THRID_BYTE_ADDR ] = ( desc.record_mask >> THRID_BYTE_OFS) & BYTE_MASK;
	datacash[RECORD_FORMAT_ADDR + FOURTH_BYTE_ADDR ] = ( desc.record_mask >> FOURTH_BYTE_OFS) & BYTE_MASK;
	eEEPROMWr(VALIDE_CODE_ADDR, datacash,REGISTER_OFFSET);
	( void )memset(datacash,0U,REGISTER_OFFSET );
	 eEEPROMRd(VALIDE_CODE_ADDR, datacash, REGISTER_OFFSET  );
	return  ( STORAGE_OK );
}

/*
 *  Функции работы с регистрами
 *
 */
void vSetRegData(uint8_t * buf, uint8_t * data, uint8_t data_type)
{
	buf[0] = data_type;
	(void) memcpy(&buf[1],data,REGISTER_SIZE - 1);
	return;
}

/*
 *  Запись регистра с указанием типа данных
 */
EERPOM_ERROR_CODE_t eEEPROMRegTypeWrite( EEPROM_ADRESS_TYPE addr, void * data, REGISTE_DATA_TYPE_t datatype )
{
	uint8_t Data[REGISTER_SIZE];
	if  (datatype ==TIME_STAMP )
	{
		vSetTimeToReg(Data,  data);
	}
	else
	{
		vSetRegData(Data,(uint8_t *) data, datatype);
	}
	return  ( eEEPROMRegWrite(addr, Data) );
}
/*
 *  Запись абстрактных 5 байт данных в регистр
 */
EERPOM_ERROR_CODE_t eEEPROMRegWrite( EEPROM_ADRESS_TYPE addr, uint8_t * data )
{
	EEPROM_ADRESS_TYPE usAddres = addr * REGISTER_SIZE + REGISTER_OFFSET ;
	if (( addr  <  DataStorageDiscriptor.register_count  ) && (USB_access==0))
	{
		uint8_t Data[REGISTER_SIZE];
		( void )memcpy(Data,data,REGISTER_SIZE);
		return ( eEEPROMWr(usAddres,&Data[0],REGISTER_SIZE));

	}
	return ( EEPROM_NOT_VALIDE_ADRESS );
}
/*
 * Чтение данных из регистра
 */
REGISTE_DATA_TYPE_t eEEPROMReadRegister( EEPROM_ADRESS_TYPE addr, void * pData )
{
	EEPROM_ADRESS_TYPE usAddres = addr * REGISTER_SIZE  + REGISTER_OFFSET ;
	uint8_t Data[REGISTER_SIZE];
	eEEPROMRd(usAddres , Data, REGISTER_SIZE);
	REGISTE_DATA_TYPE_t register_type = (Data[0] & REGISTER_TYPE_MASK);
	switch ( register_type )
	{
				case INTEGER_DATA:
				case BOOLEAN_DATA:
				case NUMBER_DATA:
				   ( void )memcpy(pData,&Data[1U],EEPROM_DATA_FRAME-1U);
					break;
				case TIME_STAMP:
					( void )memcpy(pData,Data,EEPROM_DATA_FRAME);
					((uint8_t *)pData)[0] &=  ~REGISTER_TYPE_MASK ;
					break;
				default:
					break;
	}
	return ( register_type );
}
/* блок функций быстрой записи*/
static FAST_WRITE_EEPROM_DISCRIPTOR fast_write_descriptor;


void eEEPROMConfigFastWrite(REGISTE_DATA_TYPE_t start_address)
{
	fast_write_descriptor.start_adress = start_address;
	fast_write_descriptor.cur_offset = 0;
	fast_write_descriptor.cur_length = 0;
	fast_write_descriptor.cur_addres = start_address;


}
void eEEPROMRegisterAddToFastWrite(void * data, REGISTE_DATA_TYPE_t datatype )
{
	uint8_t Data[REGISTER_SIZE];
	if  (datatype ==TIME_STAMP )
	{
			vSetTimeToReg(Data,  data);
	}
	else
	{
			vSetRegData(Data,(uint8_t *) data, datatype);
	}
	uint8_t offset =  (fast_write_descriptor.cur_addres + REGISTER_SIZE)  % SECTOR_SIZE;
	if ( offset  !=0 )
	{
		memcpy(&fast_write_descriptor.buffer[fast_write_descriptor.cur_offset],Data,REGISTER_SIZE - offset);
		eEEPROMRegisterFastWrtie();
		memcpy(fast_write_descriptor.buffer,&Data[REGISTER_SIZE - offset],offset);
		fast_write_descriptor.cur_offset = offset;
	}
	else
	{
		memcpy(&fast_write_descriptor.buffer[fast_write_descriptor.cur_offset],Data,REGISTER_SIZE);
		fast_write_descriptor.cur_offset +=REGISTER_SIZE;
	}
	fast_write_descriptor.cur_addres +=REGISTER_SIZE;

}
EERPOM_ERROR_CODE_t eEEPROMRegisterFastWrtie()
{
	return (  eEEPROMWr(fast_write_descriptor.cur_addres,fast_write_descriptor.buffer,fast_write_descriptor.cur_offset) );
}


/*
 *  Функции работы с записями
 */



RECORD_DATA_TYPE_t eGetReocrdFieldsType(uint8_t index )
{
	if (index < DataStorageDiscriptor.record_fields_count)
	{
		return ( ( DataStorageDiscriptor.record_mask >>(2*index) ) & RECORD_TYPE_MASK );
	}
	else
	{
		return (RECORD_ERROR);
	}
}
/*
 *
 */
STORAGE_ERROR vSetRecordData(uint8_t index, uint8_t * data)
{
	   uint32_t record_type  = DataStorageDiscriptor.record_mask;
	   uint16_t data_offset = 0;
	   if (index < DataStorageDiscriptor.record_fields_count)
	   {
		   for (uint8_t i=0;i<index;i++)
		   {
			   switch (record_type & RECORD_TYPE_MASK)
			   {
				  case RECORD_TIME_STAMP:
				  case RECORD_LUA:
					    data_offset +=REGISTER_SIZE;
					   	break;
				  case RECORD_BYTE:
					   	data_offset +=1;
					   	break;
				  case RECORD_SHORT:
					   	data_offset +=2;
					   	break;
				  default:
				 		break;
			   }
			   record_type= record_type >> 2;
		    }
		    switch (record_type & RECORD_TYPE_MASK)
		    {
			   	   	  case RECORD_TIME_STAMP:
			   	   		  RTC_TimeTypeDef time_buffer;
			   	   		  RTC_DateTypeDef date_buffer;
			   	   	      PDM_DATA_TIME temp_time;
			   	   		  HAL_RTC_GetTime(&hrtc, &time_buffer,  RTC_FORMAT_BIN);
			   	   		  HAL_RTC_GetDate(&hrtc, &date_buffer,  RTC_FORMAT_BIN);
			   	   		  temp_time.Day 	= date_buffer.Date;
			   	   		  temp_time.Month 	= date_buffer.Month;
			   	   		  temp_time.Year 	= date_buffer.Year;
			   	   		  temp_time.Hour 	= time_buffer.Hours;
			   	   		  temp_time.Minute 	= time_buffer.Minutes;
			   	   		  temp_time.Second 	= time_buffer.Seconds;
			   	   		  vSetTimeToReg( record_data, &temp_time);
			   	   		  break;
			   	   	  case RECORD_BYTE:
			   	      	  record_data[data_offset]  =  (uint8_t)*data;
			   	   		  break;
			   	   	  case RECORD_SHORT:
			   	   		  *((uint16_t*)&record_data[data_offset]) =  *((uint16_t*)data);
			   	   		  break;
			   	   	  case RECORD_LUA:
			   	   		  memcpy(&record_data[data_offset],data,REGISTER_SIZE );
			   	   		  break;
			   	   	  default:
			   	   		  break;
			   }
		    return (STORAGE_OK);
		   }
	   return (STORAGE_OUT_OF_RANGE);

}
/*
 *
 */
EERPOM_ERROR_CODE_t eEEPROMRecordADD()
{
	if ( ( DataStorageDiscriptor.max_record_count != 0 ) && (USB_access==0))
	    {
	    	uint16_t record_start_index  = vAddRecordToStorage();
	    	return (  eEEPROMWr(record_start_index,record_data,DataStorageDiscriptor.record_byte_size) );
	    }
		return ( EEPROM_NOT_VALIDE_ADRESS );
}





PDM_DATA_TIME vGetTimeFromReg( uint8_t * DataStorage)
{
PDM_DATA_TIME temp_time;
uint32_t date_time = DataStorage[4] | DataStorage[3]<<SECOND_BYTE_OFS | DataStorage[2]<<THRID_BYTE_OFS  | DataStorage[1]<<FOURTH_BYTE_OFS;
temp_time.Second = date_time & SECOND_MASK;
temp_time.Minute = (date_time >>MINUTE_OFS ) & MINUTE_MASK;
temp_time.Hour = (date_time >> HOUR_OFS) & HOUR_MASK;
temp_time.Year = (date_time >> YEAR_OFS) & YEAR_MASK;
temp_time.Month = (date_time >> MONTH_OFS) & MONTH_MASK;
temp_time.Day = ( (date_time >> DAY_OFS_LSB) & DAY_MASK_LSB)  | ((DataStorage[0] &  DAY_MASK_MSB)<<DAY_OFS_MSB);
return (temp_time);
}


 void vGetRegToTime( uint8_t * DataStorage, PDM_DATA_TIME * data)
{
 uint32_t date_time = DataStorage[4] | DataStorage[3]<<SECOND_BYTE_OFS | DataStorage[2]<<THRID_BYTE_OFS  | DataStorage[1]<<FOURTH_BYTE_OFS;
 data->Second = date_time & SECOND_MASK;
 data->Minute = (date_time >>MINUTE_OFS ) & MINUTE_MASK;
 data->Hour = (date_time >> HOUR_OFS) & HOUR_MASK;
 data->Year = (date_time >> YEAR_OFS) & YEAR_MASK;
 data->Month = (date_time >> MONTH_OFS) & MONTH_MASK;
 data->Day = ((date_time >> DAY_OFS_LSB) & DAY_MASK_LSB) | ((DataStorage[0] &  DAY_MASK_MSB)<<DAY_OFS_MSB);
 return;
}




uint16_t usGetEEPROMSize()
{
	return ( (DataStorageDiscriptor.access == ACCESS_ALLOWED) ? (EEPROM_SIZE) : 0 );
}



int eAccessToken( uint16_t token)
{
	if (DataStorageDiscriptor.token == 0)
	{
		SET_SHORT( ACCESS_TOKEN_ADDR,  token);
		DataStorageDiscriptor.token = token;
		eEEPROMWr(ACCESS_TOKEN_ADDR, &datacash[ACCESS_TOKEN_ADDR],2);
		DataStorageDiscriptor.access = ACCESS_ALLOWED;
	}
	else
	{
		DataStorageDiscriptor.access  = (DataStorageDiscriptor.token == token) ? ACCESS_ALLOWED : ACCESS_DENIED;
	}
	return  (DataStorageDiscriptor.access );
}

/*
 *  Функции байтового доступа к хранилищу данных
 */
/*
 *  Чтение данных из хранилища во внешний истоник
 */
uint16_t usEEPROMReadToExternData(EEPROM_ADRESS_TYPE addr, uint8_t * data, uint8_t len )
{
	if (DataStorageDiscriptor.access == ACCESS_ALLOWED)
	{
		USB_access = 1;
		uint16_t remain = usGetEEPROMSize() - addr;
		uint8_t  length = ( remain > len ) ? len : ( uint8_t )remain;
		eEEPROMRd( addr , data,  length );
	    USB_access = 0;
		return ( length );
	}
	return ( 0 );
}
/*
 * Запись данных из внешнего источника в хранилище
 */
EERPOM_ERROR_CODE_t eEEPROMWriteExternData ( uint32_t adr, const uint8_t* data, uint32_t length )
{
	if ( DataStorageDiscriptor.access == ACCESS_ALLOWED)
	{
		if ( length + adr  <= EEPROM_MAX_ADRRES )
		{
			USB_access = 1;
			eEEPROMWr( adr,(uint8_t*)data,  length );
			USB_access = 0;
			return (EEPROM_OK);
        }
		else
		{
			return (EEPROM_NOT_VALIDE_ADRESS) ;
		}
	}
	return ( EEPROM_ACCESS_ERROR );
}


/*******************************************************PRIVATE********************************************************************/
/*
 *   функция сборса хранилища
 */
static void eResetDataStorage()
{
	 uint8_t data_buffer[SECTOR_SIZE];
	 ( void )memset(data_buffer,0U,SECTOR_SIZE);
	 DataStorageDiscriptor.access = ACCESS_ALLOWED;
	 for (uint16_t i=0;i<EEPROM_SIZE;i=i+SECTOR_SIZE)
	 {
		 eEEPROMWriteExternData(i, data_buffer,SECTOR_SIZE);
	 }
	 datacash[VALIDE_CODE_ADDR ] = VALID_CODE;
	 eEEPROMWr( VALIDE_CODE_ADDR , datacash, 1 );
	 return;
}

static void vInitDescriptor()
{
	uint8_t offset = 0U;
	DataStorageDiscriptor.record_fields_count = 0U;
	DataStorageDiscriptor.register_count     = GET_REG(REGISTER_COUNT_ADDR);
	DataStorageDiscriptor.token              = GET_SHORT(ACCESS_TOKEN_ADDR);
	if ( datacash[RECORD_SIZE_ADDR ] !=0U )
	{

		DataStorageDiscriptor.record_pointer     = GET_REG(RECORD_POINTER_ADDR);
		DataStorageDiscriptor.record_count       = GET_REG(RECORD_COUNT_ADDR);
		DataStorageDiscriptor.recod_start_offset = GET_REG(REGISTER_COUNT_ADDR)*REGISTER_SIZE + REGISTER_OFFSET;
		DataStorageDiscriptor.record_byte_size = datacash[RECORD_SIZE_ADDR ];
		DataStorageDiscriptor.max_record_count   = (EEPROM_MAX_ADRRES - DataStorageDiscriptor.recod_start_offset)/DataStorageDiscriptor.record_byte_size;

		DataStorageDiscriptor.record_mask = datacash[RECORD_FORMAT_ADDR]
										|   datacash[RECORD_FORMAT_ADDR+SECOND_BYTE_ADDR]<<SECOND_BYTE_OFS
										|   datacash[RECORD_FORMAT_ADDR+2]<<THRID_BYTE_OFS
										|   datacash[RECORD_FORMAT_ADDR+FOURTH_BYTE_ADDR]<<FOURTH_BYTE_OFS;;
		uint32_t temp_data_format = DataStorageDiscriptor.record_mask;
		if ((DataStorageDiscriptor.record_mask & DATA_TYPE_MASK) == 0 )
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
		DataStorageDiscriptor.current_reccord_offset = DataStorageDiscriptor.recod_start_offset + DataStorageDiscriptor.record_byte_size * DataStorageDiscriptor.record_pointer;
	}
	else
	{
		DataStorageDiscriptor.current_reccord_offset = REGISTER_OFFSET;
		DataStorageDiscriptor.recod_start_offset = 0;
		DataStorageDiscriptor.max_record_count   = 0;
		DataStorageDiscriptor.record_fields_count = 0;
		DataStorageDiscriptor.record_count = 0;
		DataStorageDiscriptor.record_pointer = 0;
		DataStorageDiscriptor.record_mask = 0;
		//eWriteNewDescriptor(DataStorageDiscriptor);
	}
}

static uint16_t vAddRecordToStorage()
{
	uint16_t current_offset;
	if   ( (DataStorageDiscriptor.current_reccord_offset  + DataStorageDiscriptor.record_byte_size) > EEPROM_MAX_ADRRES )
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
	DataStorageDiscriptor.current_reccord_offset += DataStorageDiscriptor.record_byte_size;
	vWriteDescriptor();
	eEEPROMWr(0, datacash, REGISTER_OFFSET);
	return (current_offset);
}

static void vWriteDescriptor()
{
#if  ADDRESS_SIZE_BYTES == 2
	SET_SHORT(RECORD_COUNT_ADDR,  DataStorageDiscriptor.record_count);
	SET_SHORT(RECORD_POINTER_ADDR, DataStorageDiscriptor.record_pointer);

#else
	SET_LONG( RECORD_COUNT_ADDR,  DataStorageDiscriptor.record_count);
	SET_LONG(RECORD_POINTER_ADDR, DataStorageDiscriptor.record_pointer);
#endif
}

static void vWriteDescriptorReg( uint8_t addr, EEPROM_ADRESS_TYPE data)
{
#if  ADDRESS_SIZE_BYTES == 2
	SET_SHORT( addr,  data);
#else
	SET_LONG( addr,  data);

#endif
}

static void SET_SHORT( EEPROM_ADRESS_TYPE addr, uint16_t data)
{
	datacash[addr] =  data >> SECOND_BYTE_OFS;
	datacash[addr+1] = data & BYTE_MASK;
}


/*
 * Функци записи данных даты и времени в нужны регистр
 */
static void vSetTimeToReg( uint8_t * Data, void * data)
{
	uint32_t date_time;
	date_time = (uint32_t)( ( (PDM_DATA_TIME*) data )->Second & SECOND_MASK);
	date_time |= (uint32_t)( ( (PDM_DATA_TIME*) data)->Minute &  MINUTE_MASK ) << MINUTE_OFS ;
	date_time |= (uint32_t)( ( (PDM_DATA_TIME*)data)->Hour & HOUR_MASK ) << HOUR_OFS ;
	date_time |= (uint32_t)( ( (PDM_DATA_TIME*)data)->Year & YEAR_MASK ) << YEAR_OFS ;
	date_time |= (uint32_t)( ( (PDM_DATA_TIME*)data)->Month & MONTH_MASK  ) << MONTH_OFS ;
	date_time |= (uint32_t)( ( (PDM_DATA_TIME*)data)->Day & DAY_MASK_LSB) << DAY_OFS_LSB;
	Data[0U] = (TIME_STAMP  | ((( (PDM_DATA_TIME*)data)->Day & DAY_MASK_MSB) >> DAY_OFS_MSB ));
	Data[1U] = (uint8_t)((date_time >> FOURTH_BYTE_OFS ) & BYTE_MASK );
	Data[2U] = (uint8_t)((date_time >> THRID_BYTE_OFS) & BYTE_MASK );
	Data[THRID_BYTE_ADDR + 1U ] = (uint8_t)((date_time >> SECOND_BYTE_OFS) & BYTE_MASK );
	Data[FOURTH_BYTE_ADDR + 1U ] = (uint8_t)(date_time & BYTE_MASK );
 return;
}



