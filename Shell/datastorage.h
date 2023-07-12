#ifndef DATASTORAGE_H_
#define DATASTORAGE_H_



#include "main.h"

#define Device_ADD 0x00A0
#define VALID_CODE   0x55
#define BOOLEAN_DATA 0x51
#define INTEGER_DATA 0x52
#define NUMBER_DATA  0x53
#define TIME_STAMP   0x80
#define INVALID_CODE 0x00
#define WRITE_DATA_FRAME 0x06

/*
 * Располжение данных в дисктрипоторе
 */
#define REGISTER_VALID_CODE_ADDR 0x00
#define REGISTER_COUNT_ADDR  	 0x01
#define RECORD_COUNT_ADDR		 0x03
#define RECORD_POINTER_ADDR	     0x05
#define RECORD_SIZE_ADDR		 0x07
#define DESCRIPTOR_SIZE			 0x08

typedef enum {
	DATA_STORAGE_OK,
	DATA_STORAGE_REINIT,
} DATA_STORAGE_STATUS;

#define LOG_RECORD_SIZE_ADDR 0x02
#define REGISTER_SIZE 0x05    		 //Размер вертуального регистра

typedef enum {
	EEPROM_OK,
	EEPROM_INIT_ERROR,
	EEPROM_NOT_VALIDE_ADRESS,
	EEPROM_READ_ERROR,
	EEPROM_WRITE_ERROR,
	EEPROM_ACCESS_ERROR,
} EERPOM_ERROR_CODE_t;

typedef struct {
	uint8_t DATA[4];
	uint8_t VALID_FLAG;
} EEPROM_RECORD_t;


typedef enum {
  DS_OK,
  OUT_OF_SIZE,
} DATA_STORAGE_t;

#define EEPROM_DATA_FRAME 5U
#define EEPROM_ADRESS_SIZE 1U
#define EEPROM_MAX_ADRRES 0x7FF
#define EEPROM_TIME_OUT 100U

#define GET_ADDR_MSB( ADDR) (( ADDR >>8U ) & 0x03)

#define GET_SHORT(ADD) ( ((uint16_t)datacash[ADD] << 8 ) | datacash[ADD+1] )
//#define SET_SHORT (ADD,DATA)  datacash[ADD] = DATA >> 8, datacash[ADD+1] = DATA & 0xFF
void vEEPROMInit(I2C_HandleTypeDef * hi2c2);
int eAccessToken( uint16_t token);
int iReadEEPROM();
int iWriteEEPROM();
uint16_t usGetEEPROMSize();
uint16_t usEEPROMReadToUSB(uint16_t addr, uint8_t * data, uint8_t len );
EERPOM_ERROR_CODE_t eEEPROMReadTpye( uint16_t addr , uint8_t * data_type );
EERPOM_ERROR_CODE_t eEEPROMRead( uint16_t addr, uint8_t * data );
EERPOM_ERROR_CODE_t eEEPROMRd( uint16_t addr, uint8_t * data, uint8_t len );
EERPOM_ERROR_CODE_t eEEPROMWrite( uint16_t addr, uint8_t * data, uint8_t data_type );

#endif /* DATASTORAGE_H_ */
