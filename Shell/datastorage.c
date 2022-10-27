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

I2C_HandleTypeDef  * I2C;



void vEEPROMInit(I2C_HandleTypeDef * hi2c2)
{
 //uint8_t Data[]={0x01,0x22,0x44,0x77,0x44,0x66,0x76,0x24};
 //uint8_t rData[8] ={0x01,0,0,0,0,0,0,0};
 I2C = hi2c2;
 for (uint16_t i=0;i<EEPROM_MAX_ADRRES;i++)
 {
	 datacash[ i ] = INVALID_CODE;
 }
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



