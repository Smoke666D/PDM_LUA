/*
 * datastorage.c
 *
 *  Created on: 26 окт. 2022 г.
 *      Author: smoke
 */


#include "datastorage.h"
#include "stm32f4xx_hal.h"


I2C_HandleTypeDef  * I2C;


void vEEPROMInit(I2C_HandleTypeDef * hi2c2)
{
 uint8_t Data[]={0x01,0x22,0x44,0x77,0x44,0x66,0x76,0x24};
 uint8_t rData[8] ={0x01,0,0,0,0,0,0,0};
 I2C = hi2c2;

 HAL_I2C_Master_Transmit(I2C, Device_ADD , &Data[0], 8, 1000);
 HAL_I2C_Master_Transmit(I2C, Device_ADD , &Data[0], 1, 1000);
 HAL_I2C_Master_Receive( I2C, Device_ADD, &rData[0], 7, 1000);
 return;
}


EERPOM_ERROR_CODE_t vEEPROMRead( uint16_t addr, uint8_t * data  )
{
	EERPOM_ERROR_CODE_t res = EEPROM_NOT_VALIDE_ADRESS;
	uint8_t ucData[4];
	uint16_t usAddres = addr * EEPROM_DATA_FRAME;
	if (  usAddres  < EEPROM_MAX_ADRRES )
	{
		res = EEPROM_READ_ERROR;
		ucData[0] = (uint8_t)(usAddres & 0xFF);
		if ( HAL_I2C_Master_Transmit(I2C, Device_ADD | (( usAddres >>8 ) & 0x03) , &ucData[0], 1, 100) == HAL_OK )
		{
			if ( HAL_I2C_Master_Receive( I2C, Device_ADD, &ucData[0], EEPROM_DATA_FRAME, 100) == HAL_OK)
			{
				if (ucData[0] == VALID_CODE )
				{
					res = EEPROM_OK;
					data[0U] = ucData[0U];
					data[1U] = ucData[1U];
					data[2U] = ucData[2U];
					data[3U]  =ucData[3U];
				}
			}
		}
	}
	return ( res );
}
EERPOM_ERROR_CODE_t vEEPROMWrite( uint16_t addr, uint8_t * data )
{
	uint8_t ucData[] = {addr * EEPROM_DATA_FRAME , VALID_CODE,0,0,0,0};
	ucData[2]= data[0];
	ucData[3]= data[1];
	ucData[4]= data[2];
	ucData[5]= data[3];
	return ( (HAL_I2C_Master_Transmit(I2C, Device_ADD , &ucData[0], EEPROM_DATA_FRAME + 1U, 100) == HAL_OK ) ? EEPROM_OK : EEPROM_WRITE_ERROR );
}
