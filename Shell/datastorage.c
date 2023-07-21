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

static uint8_t datacash[EEPROM_MAX_ADRRES]		__SECTION(RAM_SECTION_CCMRAM);
static uint8_t  acces_permit					__SECTION(RAM_SECTION_CCMRAM);
static uint8_t  USB_access  = 0;

I2C_HandleTypeDef  * I2C;

#define REGISTER_SIZE		 0x05
#define VALIDE_CODE_ADDR     0x00
#define ACCESS_TOKEN_ADDR    ( VALIDE_CODE_ADDR   + 1U )
#define REGISTER_COUNT_ADDR  ( ACCESS_TOKEN_ADDR   + 2U )
#define RECORD_COUNT_ADDR	 ( REGISTER_COUNT_ADDR + 2U )
#define RECORD_POINTER_ADDR  ( RECORD_COUNT_ADDR    +2U)
#define RECORD_SIZE_ADDR     ( RECORD_POINTER_ADDR  + 2U )
#define RECORD_FORMAT_ADDR   ( RECORD_SIZE_ADDR +1U )
#define REGISTER_OFFSET      (RECORD_FORMAT_ADDR + 4U)

#define  MAX_RECORD_SIZE 	  32

static EEPROM_DISCRIPTOR DataStorageDiscriptor;
static EERPOM_ERROR_CODE_t eEEPROMWr( uint16_t addr, uint8_t * data, uint16_t len );
static DATA_STORAGE_STATUS eResetDataStorage();

static void SET_SHORT( uint16_t addr, uint16_t data);

static DATA_STORAGE_STATUS eResetDataStorage()
{

	datacash[VALIDE_CODE_ADDR ] = VALID_CODE;
	for (int i = 1 ; i< EEPROM_MAX_ADRRES;i++)
		datacash[i] = 0;
}

static DATA_STORAGE_STATUS eResetDataStorage1()
{

	datacash[VALIDE_CODE_ADDR ] = VALID_CODE;
	for (int i = 0 ; i< EEPROM_MAX_ADRRES;i++)
		datacash[i] = i;
}
#define I2C_NO_OPTION_FRAME       0xFFFF0000U /*!< XferOptions default value */


static void vInitDescriptor()
{
	uint8_t offset = 0;
	DataStorageDiscriptor.record_fields_count = 0;
	if ( datacash[RECORD_SIZE_ADDR ] !=0 )
	{
		DataStorageDiscriptor.recod_start_offset = GET_SHORT(REGISTER_COUNT_ADDR)*REGISTER_LEN + REGISTER_OFFSET;
		DataStorageDiscriptor.max_record_count   = (EEPROM_MAX_ADRRES - DataStorageDiscriptor.recod_start_offset)/datacash[RECORD_SIZE_ADDR ];
		uint32_t temp_data_format = datacash[RECORD_FORMAT_ADDR] | datacash[RECORD_FORMAT_ADDR+1]<<8 | datacash[RECORD_FORMAT_ADDR+2]<<16 | datacash[RECORD_FORMAT_ADDR+3]<<24;;
		if ((temp_data_format & 0x03) == 0 )
		{
			offset = 1;
			DataStorageDiscriptor.record_fields_count = 1;
			temp_data_format>>=2;
		}
		for (int i = offset; i < MAX_RECORD_FIELDS; i++)
		{
			if ((temp_data_format & 0x03)!=0)
			{
				DataStorageDiscriptor.record_fields_count++;
				temp_data_format>>=2;
			}
			else
				break;
		}
		DataStorageDiscriptor.current_reccord_offset = DataStorageDiscriptor.recod_start_offset + datacash[RECORD_SIZE_ADDR ] * GET_SHORT(RECORD_POINTER_ADDR);
	}
	else
	{
		DataStorageDiscriptor.current_reccord_offset = REGISTER_OFFSET;
		DataStorageDiscriptor.recod_start_offset = 0;
		DataStorageDiscriptor.max_record_count   = 0;
		DataStorageDiscriptor.record_fields_count = 0;
	}

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
		*data_type |= datacash[RECORD_FORMAT_ADDR+3]<<25;
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
		SET_SHORT(RECORD_POINTER_ADDR, 0 );
	}
	else
	{
		SET_SHORT(RECORD_POINTER_ADDR,  GET_SHORT(RECORD_POINTER_ADDR) + 1 );
	}
	if ( GET_SHORT(RECORD_COUNT_ADDR)  < DataStorageDiscriptor.max_record_count )
	{
	    SET_SHORT(RECORD_COUNT_ADDR,GET_SHORT(RECORD_COUNT_ADDR) +1 );
	}
	current_offset = DataStorageDiscriptor.current_reccord_offset;
	DataStorageDiscriptor.current_reccord_offset += datacash[RECORD_SIZE_ADDR];
	eEEPROMWr(0,&datacash,REGISTER_OFFSET);
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


 EERPOM_ERROR_CODE_t eIntiDataStorage()
{
	EERPOM_ERROR_CODE_t res = EEPROM_OK;

  //  eResetDataStorage1();
	//eEEPROMWr(VALIDE_CODE_ADDR , &datacash[0], 1000 );
	//eResetDataStorage();
	if ( eEEPROMRd(VALIDE_CODE_ADDR,&datacash[0],EEPROM_MAX_ADRRES ) != EEPROM_OK)
	{
		res = EEPROM_READ_ERROR;
    }
	else
	{


		if (datacash[VALIDE_CODE_ADDR ] != VALID_CODE)
		{
			eResetDataStorage();
			res =  (eEEPROMWr(VALIDE_CODE_ADDR , &datacash[0], EEPROM_MAX_ADRRES ) == HAL_OK ) ? EEPROM_OK : EEPROM_WRITE_ERROR ;
			if (res == EEPROM_OK)
			{
				eEEPROMRd(VALIDE_CODE_ADDR,&datacash[0], EEPROM_MAX_ADRRES);
			}
			else
				eResetDataStorage();

		}
		vInitDescriptor();

	}
	return ( res );
}

static void SET_SHORT( uint16_t addr, uint16_t data)
{
	datacash[addr] = data >> 8U;
	datacash[addr+1] = data & 0xFF;
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
		//HAL_I2C_Master_Transmit(I2C, Device_ADD | GET_ADDR_MSB( ACCESS_TOKEN_ADDR) , &datacash[ACCESS_TOKEN_ADDR], 2, EEPROM_TIME_OUT );
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

static EERPOM_ERROR_CODE_t eEEPROMWr( uint16_t addr, uint8_t * data, uint16_t len )
{
	EERPOM_ERROR_CODE_t res = EEPROM_NOT_VALIDE_ADRESS;
	uint16_t cur_addr = addr;
	uint16_t offset = 0;
	uint8_t cur_len = 0;
	uint16_t byte_to_send = len;
	if ((  addr+ len  <= EEPROM_MAX_ADRRES ) && (len!=0))
	{

	  if (len == 1)
	  {
		  ucData[0] = (addr & 0xFF);
		  ucData[1] = *data;
		  res =  (HAL_I2C_Master_Transmit(I2C, Device_ADD | GET_ADDR_MSB( addr) , &ucData, 1, EEPROM_TIME_OUT ) == HAL_OK ) ? EEPROM_OK : EEPROM_WRITE_ERROR ;
	  }
	  else
	  {
		  res = EEPROM_OK;
		   while  (byte_to_send > 0)
		  {
			  cur_len = 16 - (cur_addr % 16);
			  if (cur_len > byte_to_send)
				  cur_len = byte_to_send;
			  for (uint8_t i=0; i < cur_len; i++)
			  {
				  ucData[i+1] = data[offset +i];
			  }
			  ucData[0]= (cur_addr & 0xFF);
			  //if (HAL_I2C_IsDeviceReady() == HAL_OK;

			  if  (HAL_I2C_Master_Transmit(I2C, Device_ADD | GET_ADDR_MSB( cur_addr) , &ucData, cur_len+1, EEPROM_TIME_OUT ) != HAL_OK )
			  {
				  res =  EEPROM_WRITE_ERROR;
				  break;
			  }
			  //osDelay(5);
			  offset = offset  + cur_len;
			  byte_to_send = byte_to_send - cur_len;
			  cur_addr = cur_addr  + cur_len;
		  }
		}

	}
	return ( res );

}






EERPOM_ERROR_CODE_t eEEPROMRd( uint16_t addr, uint8_t * data, uint16_t len )
{
	EERPOM_ERROR_CODE_t res = EEPROM_NOT_VALIDE_ADRESS;

	if ( (addr +len)  <= EEPROM_MAX_ADRRES )
	{
		/*uint8_t ucTemp = (uint8_t)(addr & 0xFF);
		if ( HAL_I2C_Master_Transmit(I2C, Device_ADD | GET_ADDR_MSB(addr ), &ucTemp, EEPROM_ADRESS_SIZE , 1000) != HAL_OK )
		{
			res = EEPROM_READ_ERROR;

		}
		if ( HAL_I2C_Master_Receive( I2C, Device_ADD, &data, len , 1000) != HAL_OK)
		{

				res = EEPROM_READ_ERROR;

		}
		else
		{
			    res = EEPROM_OK;
		}*/

		/*  while  (byte_to_read > 0)
	      {
					 cur_len = 16 - (cur_addr % 16);
					  if (cur_len > byte_to_read)
						  cur_len = byte_to_read;

					  ucData[0]= (cur_addr & 0xFF);
					  if ( HAL_I2C_Master_Transmit(I2C, Device_ADD | GET_ADDR_MSB(cur_addr ), &ucData, EEPROM_ADRESS_SIZE , 1000) != HAL_OK )
					  		{
					  			break;
					  			res = EEPROM_READ_ERROR;
					  		}
					  if ( HAL_I2C_Master_Receive( I2C, Device_ADD, data[offset],  cur_len, 1000) != HAL_OK)
					  			{
					  				break;
					  					res = EEPROM_READ_ERROR;
					  			}
					  			else
					  			{
					  				res = EEPROM_OK;
					  			}


					  offset = offset  + cur_len;
					  byte_to_read = byte_to_read - cur_len;
					  cur_addr = cur_addr  + cur_len;

	      }*/

	  //for (int i =0;i<len;i=i+100)
	   {
  int i = 0;
		uint8_t ucTemp = (uint8_t)(addr+i & 0xFF);

		if ( HAL_I2C_Master_Transmit(I2C, Device_ADD | GET_ADDR_MSB(addr+i ), &ucTemp, EEPROM_ADRESS_SIZE , 1000) != HAL_OK )
		{
			//break;
			res = EEPROM_READ_ERROR;
		}
		else
		{
			if ( HAL_I2C_Master_Receive( I2C, Device_ADD, &data[i], len , 1000) != HAL_OK)
			{
				//break;
					res = EEPROM_READ_ERROR;
			}
			else
			{
				res = EEPROM_OK;
			}
		}

	  }

	}
	return ( res );
}



void vEEPROMInit(I2C_HandleTypeDef * hi2c2)
{
 //uint8_t Data[]={0x01,0x22,0x44,0x77,0x44,0x66,0x76,0x24};
 //uint8_t rData[8] ={0x01,0,0,0,0,0,0,0};
 I2C = hi2c2;
 acces_permit = 0;

 //HAL_I2C_Master_Transmit(I2C, Device_ADD , &Data[0], 8, 1000);
 //HAL_I2C_Master_Transmit(I2C, Device_ADD , &Data[0], 1, 1000);
 //HAL_I2C_Master_Receive( I2C, Device_ADD, &rData[0], 7, 1000);
 return;
}




EERPOM_ERROR_CODE_t eEEPROMReadRegTpye( uint16_t addr, uint8_t * data_type )
{
	EERPOM_ERROR_CODE_t res = EEPROM_NOT_VALIDE_ADRESS;
	uint16_t usAddres = addr * REGISTER_LEN + REGISTER_OFFSET ;
    if (( addr  < GET_SHORT(REGISTER_COUNT_ADDR) ) && (USB_access==0))
	{
    	data_type[0] = datacash[usAddres];
		res = EEPROM_OK;
	}
	return ( res );
}
/*
 *
 */
EERPOM_ERROR_CODE_t eEEPROMRegRead( uint16_t addr, uint8_t * data )
{
	EERPOM_ERROR_CODE_t res = EEPROM_NOT_VALIDE_ADRESS;
	uint16_t usAddres = addr * REGISTER_LEN + REGISTER_OFFSET ;
	if (( addr  < GET_SHORT(REGISTER_COUNT_ADDR) ) && (USB_access==0))
	{
		for (uint8_t i=0U; i < EEPROM_DATA_FRAME-1U; i++ )
		{
			data[i] = datacash[usAddres+i+1U];
		}
		res = EEPROM_OK;
	}
	return ( res );
}
/*
 *
 */
EERPOM_ERROR_CODE_t eEEPROMRegWrite( uint16_t addr, uint8_t * data )
{
	EERPOM_ERROR_CODE_t res = EEPROM_NOT_VALIDE_ADRESS;
	uint16_t usAddres = addr * REGISTER_LEN + REGISTER_OFFSET ;
	if (( addr  < GET_SHORT(REGISTER_COUNT_ADDR) ) && (USB_access==0))
	{
		uint8_t Data[WRITE_DATA_FRAME];
		for (uint8_t i=0;i<EEPROM_DATA_FRAME; i++)
		{
			Data[i] = data[i];
		}
		uint8_t newdata = 0;
		for (uint8_t i=0U; i < REGISTER_LEN; i++ )
		{
			if (datacash[usAddres + i ] != Data[i])
			{
				newdata = 1;
				break;
			}
		}
		if (newdata)
		{
			res = eEEPROMWr(usAddres,&Data[0],WRITE_DATA_FRAME);
			//ucData[0U] = (usAddres & 0xFF);
			//res =  (HAL_I2C_Master_Transmit(I2C, Device_ADD | GET_ADDR_MSB( usAddres ) , &ucData[0],WRITE_DATA_FRAME, EEPROM_TIME_OUT ) == HAL_OK ) ? EEPROM_OK : EEPROM_WRITE_ERROR ;
			if (res == EEPROM_OK)
			{
				for (uint8_t i=0U; i < REGISTER_LEN; i++ )
				{
					datacash[usAddres + i ] = ucData[i + 1U];
			    }
		     }
		}



	}
	return ( res );

}



