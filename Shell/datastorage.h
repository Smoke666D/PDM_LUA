#ifndef DATASTORAGE_H_
#define DATASTORAGE_H_



#include "main.h"

/*
 *  Настрйоки драйвера
 */
#define VALID_CODE   0x33

#define WRITE_DATA_FRAME 0x05
#define REGISTER_SIZE 0x05    		 //Размер вертуального регистра

/*
 *  Внутрение константы
 */
#define DATA_TYPE_MASK 0x03
#define REGISTER_SIZE		 0x05
#define VALIDE_CODE_ADDR     0x00
#define ACCESS_TOKEN_ADDR    ( VALIDE_CODE_ADDR     +sizeof(uint8_t) )
#define REGISTER_COUNT_ADDR  ( ACCESS_TOKEN_ADDR    +sizeof(uint16_t) )
#define RECORD_COUNT_ADDR	 ( REGISTER_COUNT_ADDR  +sizeof(EEPROM_ADRESS_TYPE) )
#define RECORD_POINTER_ADDR  ( RECORD_COUNT_ADDR    +sizeof(EEPROM_ADRESS_TYPE) )
#define RECORD_SIZE_ADDR     ( RECORD_POINTER_ADDR  +sizeof(EEPROM_ADRESS_TYPE))
#define RECORD_FORMAT_ADDR   ( RECORD_SIZE_ADDR    + sizeof(uint8_t) )
#define REGISTER_OFFSET      (RECORD_FORMAT_ADDR + 4U)
#define EEPROM_DATA_FRAME 5U
#define  MAX_RECORD_SIZE 	  16
#define REGISTER_TYPE_MASK    0xF0

#define SECOND_MASK  0x3F
#define MINUTE_MASK  0x3F
#define MINUTE_OFS     6
#define HOUR_MASK    0x1F
#define HOUR_OFS      12
#define YEAR_MASK    0x7F
#define YEAR_OFS      17
#define MONTH_MASK    0x0F
#define MONTH_OFS      24
#define DAY_MASK_LSB    0x0F
#define DAY_OFS_LSB      28
#define DAY_MASK_MSB    0x01
#define DAY_OFS_MSB      4
#define SECOND_BYTE_OFS   8
#define THRID_BYTE_OFS   16
#define FOURTH_BYTE_OFS   24

typedef enum
{
 BOOLEAN_DATA = 0x10,
 INTEGER_DATA = 0x40,
 NUMBER_DATA = 0x20,
 TIME_STAMP  = 0x80,
 INVALID_CODE = 0x00,
} REGISTE_DATA_TYPE_t;

//#define GET_ADDR_MSB_R( ADDR) ( ( ADDR >>7U ) & 0x0E )
#if ADDRESS_SIZE_BYTES == 2
	#define GET_REG(ADD) ( GET_SHORT(ADD) )
#else
#define GET_REG(ADD) ( GET_LONG(ADD) )
#endif


#define GET_LONG(ADD) ( ((uint32_t)datacash[ADD] << 24 ) | (uint32_t)(datacash[ADD+1]<<16) | (uint32_t)(datacash[ADD+2]<<8) | (uint32_t)(datacash[ADD+3]) )
#define GET_SHORT(ADD) ( ((uint16_t)datacash[ADD] << 8 ) | datacash[ADD+1] )
/*
 * Располжение данных в дисктрипоторе
 */




typedef enum {
	DATA_STORAGE_OK,
	DATA_STORAGE_REINIT,
} DATA_STORAGE_STATUS;

#define LOG_RECORD_SIZE_ADDR 0x02


typedef struct {
	uint8_t  record_fields_count;
	EEPROM_ADRESS_TYPE recod_start_offset;
	EEPROM_ADRESS_TYPE max_record_count;
	EEPROM_ADRESS_TYPE current_reccord_offset;
	EEPROM_ADRESS_TYPE record_count;
	EEPROM_ADRESS_TYPE register_count;
	EEPROM_ADRESS_TYPE record_pointer;

} EEPROM_DISCRIPTOR;




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








void vEEPROMInit(I2C_HandleTypeDef * hi2c2);
EERPOM_ERROR_CODE_t eIntiDataStorage();
EERPOM_ERROR_CODE_t eEEPROMRegTypeWrite( EEPROM_ADRESS_TYPE addr, void * data, uint8_t datatype );
EERPOM_ERROR_CODE_t eEEPROMRegWrite( EEPROM_ADRESS_TYPE addr, uint8_t * data );
void vSetRecordData(uint8_t index, uint8_t * data);
EERPOM_ERROR_CODE_t eEEPROMRecordADD();
REGISTE_DATA_TYPE_t eEEPROMReadRegister( EEPROM_ADRESS_TYPE addr, void * pData );
void vGetRegToTime( uint8_t * DataStorage, PDM_DATA_TIME * data);


int eAccessToken( uint16_t token);
int iReadEEPROM();
int iWriteEEPROM();
uint16_t usGetEEPROMSize();
uint16_t usEEPROMReadToUSB(uint16_t addr, uint8_t * data, uint8_t len );
void vEEPROMCheckRecord( uint32_t * data_type, uint8_t * record_szie );
EERPOM_ERROR_CODE_t eEEPROMRd( uint16_t addr, uint8_t * data, uint16_t len );


#endif /* DATASTORAGE_H_ */
