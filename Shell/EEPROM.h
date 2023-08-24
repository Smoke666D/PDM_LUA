/*
 * EEPROM.h
 *
 *  Created on: Aug 1, 2023
 *      Author: i.dymov
 */

#ifndef EEPROM_H_
#define EEPROM_H_

#include "main.h"
#include "cmsis_os.h"
#include "string.h"
/*
 *  Настройки драйвера
 */

#define Device_ADD 0x00A0
#define EEPROM_SIZE 2048U
#define EEPROM_ADRESS_TYPE uint16_t
#define SECTOR_SIZE 16U
#define EEPROM_TIME_OUT 100U
#define EEPROM_MAX_ADRRES 0x7FF
#define SECTOR_WRITE_TIME_ms 5
#define ADDRESS_SIZE_BYTES  2
#define ADDRESS_DATA	    (ADDRESS_SIZE_BYTES - 1)
/*   алгоритм получания данных для старшего байта адреса  */
#define BYTE_SHIFT		    8U
#define MSB_SHIFT           0x07
#define MSB_MASK			0x0E
#define GET_ADDR_MSB( ADDR) (( ADDR >>MSB_SHIFT ) & MSB_MASK )
/*
 * Cистемные константы
 */
#define ADDRES_MASK        0xFF
#define ADDRES_INDEX	   0x00


typedef enum {
	EEPROM_OK,
	EEPROM_INIT_ERROR,
	EEPROM_NOT_VALIDE_ADRESS,
	EEPROM_READ_ERROR,
	EEPROM_WRITE_ERROR,
	EEPROM_ACCESS_ERROR,
} EERPOM_ERROR_CODE_t;



uint16_t usGetEEPROMSize();
EERPOM_ERROR_CODE_t eEEPROMWr( EEPROM_ADRESS_TYPE addr, uint8_t * data, EEPROM_ADRESS_TYPE len );
void eEEPROM(I2C_HandleTypeDef * hi2c2);
EERPOM_ERROR_CODE_t eEEPROMRd( EEPROM_ADRESS_TYPE addr, uint8_t * data,  EEPROM_ADRESS_TYPE len );



#endif /* EEPROM_H_ */
