/*
 * EEPROM.c
 *
 *  Created on: Aug 1, 2023
 *      Author: i.dymov
 */

#include "EEPROM.h"

static uint8_t sector_buffer[SECTOR_SIZE + ADDRESS_DATA];
static I2C_HandleTypeDef  * I2C;


void eEEPROM(I2C_HandleTypeDef * hi2c2)
{
	I2C = hi2c2;
}


 EERPOM_ERROR_CODE_t eEEPROMWr(  EEPROM_ADRESS_TYPE addr, uint8_t * data, EEPROM_ADRESS_TYPE len )
{
	EERPOM_ERROR_CODE_t res = EEPROM_NOT_VALIDE_ADRESS;
	if ((  addr+ len  <= EEPROM_SIZE ) && (len!=0))
	{


	  if (len == sizeof(uint8_t))
	  {
		  for (uint8_t i = 0; i< ADDRESS_DATA; i++ )
		  {
		 		  sector_buffer[i] = (( addr >> BYTE_SHIFT*i ) & ADDRES_MASK ) ;
		  }
		  sector_buffer[ADDRESS_DATA] = *data;
		  res =  (HAL_I2C_Master_Transmit(I2C, Device_ADD | GET_ADDR_MSB( addr) , (uint8_t *) sector_buffer, ADDRESS_DATA + sizeof(uint8_t) , EEPROM_TIME_OUT ) == HAL_OK ) ? EEPROM_OK : EEPROM_WRITE_ERROR ;
	  }
	  else
	  {
		  res = EEPROM_OK;
		  EEPROM_ADRESS_TYPE cur_len;
		  EEPROM_ADRESS_TYPE byte_to_send = len;
		  EEPROM_ADRESS_TYPE offset = 0;
		  EEPROM_ADRESS_TYPE cur_addr = addr;
		  while  (byte_to_send > 0)
		  {
			  cur_len = SECTOR_SIZE - ( cur_addr % SECTOR_SIZE );
			  if ( cur_len > byte_to_send )  cur_len = byte_to_send;
			  ( void )memcpy( &sector_buffer[ADDRESS_DATA], &data[offset], cur_len );
			  for (uint8_t i = 0; i< ADDRESS_DATA; i++ )
			  {
			 		  sector_buffer[i] = (( cur_addr >> BYTE_SHIFT*i ) & ADDRES_MASK ) ;
			  }
			  //
			 // res =  EEPROM_WRITE_ERROR;
			 // for (int i =0; i<5;i++)
			//  {


				  if  (HAL_I2C_Master_Transmit(I2C, Device_ADD | GET_ADDR_MSB( cur_addr) ,(uint8_t *) sector_buffer,  cur_len + ADDRESS_DATA , EEPROM_TIME_OUT ) != HAL_OK )
				  {
					//  vTaskDelay(1);
					  res =  EEPROM_WRITE_ERROR;
					//  break;
				  }
				//  else
				//  {
				//	  res =  EEPROM_OK;
				//  }
			//  }
			  vTaskDelay(5);
			 // HAL_I2C_IsDeviceReady(I2C, Device_ADD,20,1);
			  offset = offset  + cur_len;
			  byte_to_send = byte_to_send - cur_len;
			  cur_addr = cur_addr  + cur_len;
		  }
		}
	}
	return ( res );
}

 /*
 * Функция чтения блока данных из EEPROM
 */
 EERPOM_ERROR_CODE_t eEEPROMRd(  EEPROM_ADRESS_TYPE addr, uint8_t * data,   EEPROM_ADRESS_TYPE len )
 {
 	if ( (addr +len)  <= EEPROM_SIZE)
 	{
 		uint8_t ucTemp[ADDRESS_DATA];
 		for (uint8_t i = 0; i< ADDRESS_DATA; i++ )
 		{
 			ucTemp[i] = (( addr >> BYTE_SHIFT*i ) & ADDRES_MASK ) ;
 	    }

 		if ( HAL_I2C_Master_Transmit(I2C, Device_ADD | GET_ADDR_MSB( addr ),(uint8_t *) &ucTemp, ADDRESS_DATA ,EEPROM_TIME_OUT ) != HAL_OK )
 		{
 			return ( EEPROM_READ_ERROR );
 		}
 		else
 		{
 			return ( ( HAL_I2C_Master_Receive( I2C, Device_ADD, data, len ,EEPROM_TIME_OUT) != HAL_OK) ? EEPROM_READ_ERROR : EEPROM_OK );
 	    }
 	}
 	return ( EEPROM_NOT_VALIDE_ADRESS );
 }


