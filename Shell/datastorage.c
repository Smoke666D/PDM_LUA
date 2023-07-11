/*
 * datastorage.c
 *
 *  Created on: 26 окт. 2022 г.
 *      Author: smoke
 */


#include "datastorage.h"
#include "stm32f4xx_hal.h"
#include "system.h"

static uint8_t datacash[EEPROM_MAX_ADRRES]		__SECTION(RAM_SECTION_CCMRAM);
static uint16_t * usRegisterCount 				__SECTION(RAM_SECTION_CCMRAM);
static uint8_t * pRecordSize 					__SECTION(RAM_SECTION_CCMRAM);
static uint8_t   ucmRecordFormat[4];			__SECTION(RAM_SECTION_CCMRAM);
static uint16_t  *usRecordPointer				__SECTION(RAM_SECTION_CCMRAM);
static uint16_t  *pRecordCount					__SECTION(RAM_SECTION_CCMRAM);
static uint8_t  acces_permit					__SECTION(RAM_SECTION_CCMRAM);

I2C_HandleTypeDef  * I2C;

#define REGISTER_SIZE		 0x05
#define VALIDE_CODE_ADDR     0x00
#define ACCESS_TOKEN_ADDR    ( VALIDE_CODE_ADDR + 1U )
#define REGISTER_COUNT_ADDR  ( ACCESS_TOKEN_ADDR + 2U )
#define RECORD_COUNT_ADDR	 ( REGISTER_COUNT_ADDR + 2U )
#define RECORD_POINTER_ADDR  (RECORD_COUNT_ADDR +2U)
#define RECORD_SIZE_ADDR     ( RECORD_POINTER_ADDR  + 2U )
#define RECORD_FORMAT_ADDR   ( RECORD_SIZE_ADDR +2U )
#define REGISTER_OFFSET      (RECORD_FORMAT_ADDR + 4U)
#define  MAX_RECORD_SIZE 	  32

static EERPOM_ERROR_CODE_t eEEPROMWr( uint16_t addr, uint8_t * data, uint8_t len );
static DATA_STORAGE_STATUS eResetDataStorage();
static EERPOM_ERROR_CODE_t eIntiDataStorage();
static void SET_SHORT( uint16_t addr, uint16_t data);

static DATA_STORAGE_STATUS eResetDataStorage()
{

	datacash[VALIDE_CODE_ADDR ] = VALID_CODE;
	SET_SHORT(ACCESS_TOKEN_ADDR, 0U);
	*usRegisterCount = 0;
	*usRecordPointer = 0;
	*pRecordCount	 = 0;
	*pRecordSize     = 0;
}


static EERPOM_ERROR_CODE_t eIntiDataStorage()
{
	EERPOM_ERROR_CODE_t res = EEPROM_OK;
	/*Читаем всю память EEPROM в кэш память*/
	if ( HAL_I2C_Master_Receive( I2C, Device_ADD, &datacash[0], EEPROM_MAX_ADRRES, EEPROM_TIME_OUT) != HAL_OK)
	{
		res = EEPROM_READ_ERROR;
    }
	else
	{
		/*Если не совпадает код, то обнуляем данные*/
		if (datacash[VALIDE_CODE_ADDR ] != VALID_CODE)
		{
			eResetDataStorage();
			eEEPROMWr(VALIDE_CODE_ADDR,&datacash[VALIDE_CODE_ADDR],REGISTER_OFFSET );
		}

		//usRegisterCount =  (uint16_t * )&datacash[REGISTER_COUNT_ADDR];
		//usRecordPointer =  (uint16_t * )&datacash[RECORD_POINTER_ADDR];
		//pRecordCount  =    (uint16_t * )&datacash[RECORD_COUNT_ADDR];
		//pRecordSize   =    (uint16_t * )&datacash[RECORD_SIZE_ADDR ];
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
	return (EEPROM_MAX_ADRRES+1);
}


int eAccessToken( uint16_t token)
{
	if (GET_SHORT(ACCESS_TOKEN_ADDR) == 0)
	{
		SET_SHORT( ACCESS_TOKEN_ADDR,  token);
		HAL_I2C_Master_Transmit(I2C, Device_ADD | GET_ADDR_MSB( ACCESS_TOKEN_ADDR) , &datacash[ACCESS_TOKEN_ADDR], 2, EEPROM_TIME_OUT );
		acces_permit  = 1;
	}
	else
	{
		acces_permit = (GET_SHORT(ACCESS_TOKEN_ADDR) == token) ? 1 : 0;
	}
	return  (acces_permit);
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
		uint8_t* source = &( data )[addr];
		uint16_t remain = usGetEEPROMSize() - addr;
		uint8_t  length = ( remain > len ) ? len : ( uint8_t )remain;
	    ( void )memcpy( data, source, length );
		return ( length );
	}
	return ( 0 );
}


static EERPOM_ERROR_CODE_t eEEPROMWr( uint16_t addr, uint8_t * data, uint8_t len )
{
	EERPOM_ERROR_CODE_t res = EEPROM_NOT_VALIDE_ADRESS;
	uint16_t usAddres = addr * EEPROM_DATA_FRAME +len;
	if (  usAddres  < EEPROM_MAX_ADRRES )
	{
		uint8_t ucData[len];
		for (uint8_t i=0;i<len; i++)
		{
			ucData[i] = data[i];
		}
		res =  (HAL_I2C_Master_Transmit(I2C, Device_ADD | GET_ADDR_MSB( usAddres ) , &ucData[0], len, EEPROM_TIME_OUT ) == HAL_OK ) ? EEPROM_OK : EEPROM_WRITE_ERROR ;
	}
	return ( res );

}


EERPOM_ERROR_CODE_t eEEPROMRd( uint16_t addr, uint8_t * data, uint8_t len )
{
	EERPOM_ERROR_CODE_t res = EEPROM_NOT_VALIDE_ADRESS;
	uint16_t usAddres = addr +len;
	if (  usAddres  < EEPROM_MAX_ADRRES )
	{
		if (datacash[usAddres] == INVALID_CODE )
		{
			uint8_t ucTemp = (uint8_t)(usAddres & 0xFF);
			if ( HAL_I2C_Master_Transmit(I2C, Device_ADD | GET_ADDR_MSB( usAddres ) , &ucTemp, EEPROM_ADRESS_SIZE , EEPROM_TIME_OUT) != HAL_OK )
			{
				res = EEPROM_READ_ERROR;
			}
			else
			{
				if ( HAL_I2C_Master_Receive( I2C, Device_ADD, &datacash[usAddres], EEPROM_DATA_FRAME, EEPROM_TIME_OUT) != HAL_OK)
				{
					res = EEPROM_READ_ERROR;
				}
			}
		}
		if (datacash[usAddres]!= INVALID_CODE)
		{
			for (uint8_t i=0U; i < EEPROM_DATA_FRAME-1U; i++ )
			{
				data[i] = datacash[usAddres+i+1U];
			}
			res = EEPROM_OK;
		}
	}
	return ( res );
}










void vEEPROMInit(I2C_HandleTypeDef * hi2c2)
{
 //uint8_t Data[]={0x01,0x22,0x44,0x77,0x44,0x66,0x76,0x24};
 //uint8_t rData[8] ={0x01,0,0,0,0,0,0,0};
 I2C = hi2c2;
 eIntiDataStorage();
 acces_permit = 0;
 //HAL_I2C_Master_Transmit(I2C, Device_ADD , &Data[0], 8, 1000);
 //HAL_I2C_Master_Transmit(I2C, Device_ADD , &Data[0], 1, 1000);
 //HAL_I2C_Master_Receive( I2C, Device_ADD, &rData[0], 7, 1000);
 return;
}
/*
 *
 */
EERPOM_ERROR_CODE_t eEEPROMReadTpye( uint16_t addr, uint8_t * data_type )
{
	EERPOM_ERROR_CODE_t res = EEPROM_NOT_VALIDE_ADRESS;
	uint16_t usAddres = addr * EEPROM_DATA_FRAME;
	if (  usAddres  < EEPROM_MAX_ADRRES )
	{
		if (datacash[usAddres] == INVALID_CODE )
		{
			uint8_t ucTemp = (uint8_t)(usAddres & 0xFF);
			if ( HAL_I2C_Master_Transmit(I2C, Device_ADD | GET_ADDR_MSB( usAddres ) , &ucTemp, EEPROM_ADRESS_SIZE , EEPROM_TIME_OUT) != HAL_OK )
			{
				res = EEPROM_READ_ERROR;
			}
			else
			{
				if ( HAL_I2C_Master_Receive( I2C, Device_ADD, &datacash[usAddres], EEPROM_DATA_FRAME, EEPROM_TIME_OUT) != HAL_OK)
				{
					res = EEPROM_READ_ERROR;
				}
			}
		}
		if (datacash[usAddres]!= INVALID_CODE)
		{
			*data_type =datacash[usAddres];
			res = EEPROM_OK;
		}
	}
	return ( res );
}

/*
 *
 */
EERPOM_ERROR_CODE_t eEEPROMRead( uint16_t addr, uint8_t * data )
{
	EERPOM_ERROR_CODE_t res = EEPROM_NOT_VALIDE_ADRESS;
	uint16_t usAddres = addr * EEPROM_DATA_FRAME;
	if (  usAddres  < EEPROM_MAX_ADRRES )
	{
		if (datacash[usAddres] == INVALID_CODE )
		{
			uint8_t ucTemp = (uint8_t)(usAddres & 0xFF);
			if ( HAL_I2C_Master_Transmit(I2C, Device_ADD | GET_ADDR_MSB( usAddres ) , &ucTemp, EEPROM_ADRESS_SIZE , EEPROM_TIME_OUT) != HAL_OK )
			{
				res = EEPROM_READ_ERROR;
			}
			else
			{
				if ( HAL_I2C_Master_Receive( I2C, Device_ADD, &datacash[usAddres], EEPROM_DATA_FRAME, EEPROM_TIME_OUT) != HAL_OK)
				{
					res = EEPROM_READ_ERROR;
				}
			}
		}
		if (datacash[usAddres]!= INVALID_CODE)
		{
			for (uint8_t i=0U; i < EEPROM_DATA_FRAME-1U; i++ )
			{
				data[i] = datacash[usAddres+i+1U];
			}
			res = EEPROM_OK;
		}
	}
	return ( res );
}
/*
 *
 */
EERPOM_ERROR_CODE_t eEEPROMWrite( uint16_t addr, uint8_t * data, uint8_t data_type )
{
	EERPOM_ERROR_CODE_t res = EEPROM_NOT_VALIDE_ADRESS;
	uint16_t usAddres = addr * EEPROM_DATA_FRAME;
	if (  usAddres  < EEPROM_MAX_ADRRES )
	{
		uint8_t ucData[WRITE_DATA_FRAME];
		for (uint8_t i=0;i<EEPROM_DATA_FRAME-1U; i++)
		{
			ucData[i+2U] = data[i];
		}
		ucData[0U] = (uint8_t)( usAddres & 0xFF );
		ucData[1U] = data_type;
		res =  (HAL_I2C_Master_Transmit(I2C, Device_ADD | GET_ADDR_MSB( usAddres ) , &ucData[0],WRITE_DATA_FRAME, EEPROM_TIME_OUT ) == HAL_OK ) ? EEPROM_OK : EEPROM_WRITE_ERROR ;
		if (res == EEPROM_OK)
		{
			for (uint8_t i=0U; i < EEPROM_DATA_FRAME-1U; i++ )
			{
				 datacash[usAddres] = ucData[i + 1U];
			}
		}
		else
		{
			datacash[usAddres]  = INVALID_CODE;
		}
	}
	return ( res );

}
/*
 *
 */



