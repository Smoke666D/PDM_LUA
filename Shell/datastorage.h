#ifndef DATASTORAGE_H_
#define DATASTORAGE_H_



#include "main.h"

#define Device_ADD 0x00A0
#define VALID_CODE   0x85
#define BOOLEAN_DATA 0x10
#define INTEGER_DATA 0x40
#define NUMBER_DATA  0x20
#define TIME_STAMP   0x80
#define INVALID_CODE 0x00
#define WRITE_DATA_FRAME 0x05
#define REGISTER_LEN 0x05

/*
 * Располжение данных в дисктрипоторе
 */
#define REGISTER_VALID_CODE_ADDR 0x00
#define REGISTER_COUNT_ADDR  	 0x01
#define RECORD_COUNT_ADDR		 0x03
#define RECORD_POINTER_ADDR	     0x05
#define RECORD_SIZE_ADDR		 0x07
#define DESCRIPTOR_SIZE			 0x08
#define MAX_RECORD_FIELDS        16U


typedef enum {
	DATA_STORAGE_OK,
	DATA_STORAGE_REINIT,
} DATA_STORAGE_STATUS;

#define LOG_RECORD_SIZE_ADDR 0x02
#define REGISTER_SIZE 0x05    		 //Размер вертуального регистра

typedef struct {
	uint8_t record_fields_count;
	uint16_t recod_start_offset;
	uint16_t max_record_count;
	uint16_t current_reccord_offset;


} EEPROM_DISCRIPTOR;

typedef enum {
	RECORD_IS_CONFIG,
	RECORD_NOT_CONFIG,
} STORAGE_STATUS;

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

typedef struct {
	uint8_t Second;
	uint8_t Minute;
	uint8_t Hour;
	uint8_t Day;
	uint8_t Month;
	uint8_t Year;
} PDM_DATA_TIME;

typedef enum {
  DS_OK,
  OUT_OF_SIZE,
} DATA_STORAGE_t;

#define EEPROM_DATA_FRAME 5U
#define EEPROM_ADRESS_SIZE 1U
#define EEPROM_MAX_ADRRES 0x7FF
#define EEPROM_TIME_OUT 100U

#define GET_ADDR_MSB( ADDR) (( ADDR >>7U ) & 0x0E)
//#define GET_ADDR_MSB_R( ADDR) ( ( ADDR >>7U ) & 0x0E )

#define GET_SHORT(ADD) ( ((uint16_t)datacash[ADD] << 8 ) | datacash[ADD+1] )
//#define SET_SHORT (ADD,DATA)  datacash[ADD] = DATA >> 8, datacash[ADD+1] = DATA & 0xFF
void vEEPROMInit(I2C_HandleTypeDef * hi2c2);
int eAccessToken( uint16_t token);
int iReadEEPROM();
int iWriteEEPROM();
uint16_t usGetEEPROMSize();
EERPOM_ERROR_CODE_t eIntiDataStorage();
uint16_t usEEPROMReadToUSB(uint16_t addr, uint8_t * data, uint8_t len );
EERPOM_ERROR_CODE_t eEEPROMReadRegTpye( uint16_t addr , uint8_t * data_type );
void vEEPROMCheckRecord( uint32_t * data_type, uint8_t * record_szie );
EERPOM_ERROR_CODE_t eEEPROMRead( uint16_t addr, uint8_t * data );
EERPOM_ERROR_CODE_t eEEPROMRd( uint16_t addr, uint8_t * data, uint16_t len );
EERPOM_ERROR_CODE_t eEEPROMRegWrite( uint16_t addr, uint8_t * data );
void vSetTimeToReg( uint8_t * DataStorage, PDM_DATA_TIME data);



STORAGE_STATUS eGetRecordsStatus();
#endif /* DATASTORAGE_H_ */
