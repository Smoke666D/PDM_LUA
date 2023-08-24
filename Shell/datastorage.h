#ifndef DATASTORAGE_H_
#define DATASTORAGE_H_



#include "main.h"

/*
 *  Настрйоки драйвера
 */

#define REGISTER_VALID_CODE_ADDR 0x00
#define DESCRIPTOR_SIZE			 0x08
#define MAX_RECORD_FIELDS        16U

#define VALID_CODE   0x66
#define WRITE_DATA_FRAME 0x05
#define REGISTER_SIZE 0x05    		 //Размер вертуального регистра
#define EXTERN_READ_BUFFER_SIZE        100 //Размер буфера для чтения данных во внешний источник
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
#define RECORD_TYPE_MASK	  0x03
#define BYTE_MASK             0xFF


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
#define DAY_MASK_MSB    0x10
#define DAY_OFS_MSB      4


#define SECOND_BYTE_OFS   8
#define SECOND_BYTE_ADDR  1
#define THRID_BYTE_OFS    16
#define THRID_BYTE_ADDR   2
#define FOURTH_BYTE_OFS   24
#define FOURTH_BYTE_ADDR  3



typedef enum
{
 BOOLEAN_DATA = 0x10,
 INTEGER_DATA = 0x40,
 NUMBER_DATA = 0x20,
 TIME_STAMP  = 0x80,
 INVALID_CODE = 0x00,
} REGISTE_DATA_TYPE_t;

typedef enum
{
	RECORD_TIME_STAMP = 0x00,
	RECORD_BYTE		  = 0x01,
	RECORD_SHORT      = 0x02,
	RECORD_LUA		  = 0x03,
	RECORD_ERROR	  = 0xFF,

} RECORD_DATA_TYPE_t;

typedef enum
{
  STORAGE_OK,
  STORAGE_OUT_OF_RANGE,
  STORAGE_NEW,
} STORAGE_ERROR;

//#define GET_ADDR_MSB_R( ADDR) ( ( ADDR >>7U ) & 0x0E )
#if ADDRESS_SIZE_BYTES == 2
	#define GET_REG(ADD) ( GET_SHORT(ADD) )
#else
#define GET_REG(ADD) ( GET_LONG(ADD) )
#endif

#define GET_LONG(ADD) ( ((uint32_t)datacash[ADD] << 24 ) | (uint32_t)(datacash[ADD+1]<<16) | (uint32_t)(datacash[ADD+2]<<8) | (uint32_t)(datacash[ADD+3]) )
#define GET_SHORT(ADD) ( ((uint16_t)datacash[ADD] << 8 ) | datacash[ADD+1] )

typedef enum {
	ACCESS_DENIED = 0,
	ACCESS_ALLOWED = 1,
} STRORAGE_ACCESS_TYPE_t;

typedef enum {
	DATA_STORAGE_OK,
	DATA_STORAGE_REINIT,
} DATA_STORAGE_STATUS;

#define LOG_RECORD_SIZE_ADDR 0x02

typedef struct {
	uint8_t  record_fields_count;
	uint16_t  		   token;
	EEPROM_ADRESS_TYPE recod_start_offset;
	EEPROM_ADRESS_TYPE max_record_count;
	EEPROM_ADRESS_TYPE current_reccord_offset;
	EEPROM_ADRESS_TYPE record_count;
	EEPROM_ADRESS_TYPE register_count;
	EEPROM_ADRESS_TYPE record_pointer;
	uint32_t            record_mask;
	//uint16_t			access_code;
	uint8_t 			record_byte_size;
	STRORAGE_ACCESS_TYPE_t access;
} EEPROM_DISCRIPTOR;

typedef struct {
	uint8_t  buffer[SECTOR_SIZE];
	EEPROM_ADRESS_TYPE start_adress;
	EEPROM_ADRESS_TYPE cur_addres;
	EEPROM_ADRESS_TYPE cur_offset;
	EEPROM_ADRESS_TYPE register_count;
	uint16_t cur_length;
} FAST_WRITE_EEPROM_DISCRIPTOR;


typedef struct {
	uint8_t Second;
	uint8_t Minute;
	uint8_t Hour;
	uint8_t Day;
	uint8_t Month;
	uint8_t Year;
} PDM_DATA_TIME;

STORAGE_ERROR eCreateDataStorage(EEPROM_ADRESS_TYPE reg_count, uint8_t * record_format_data, uint8_t record_count);
void vEEPROMInit(I2C_HandleTypeDef * hi2c2);
EERPOM_ERROR_CODE_t eIntiDataStorage();





EERPOM_ERROR_CODE_t eEEPROMRd( uint16_t addr, uint8_t * data, uint16_t len );
EERPOM_ERROR_CODE_t eEEPROMRegWrite( uint16_t addr, uint8_t * data );

EERPOM_ERROR_CODE_t eEEPROMRegTypeWrite( EEPROM_ADRESS_TYPE addr, void * data, uint8_t datatype );
EERPOM_ERROR_CODE_t eEEPROMRegWrite( EEPROM_ADRESS_TYPE addr, uint8_t * data );
STORAGE_ERROR vSetRecordData(uint8_t index, uint8_t * data);
EERPOM_ERROR_CODE_t eEEPROMRecordADD();
REGISTE_DATA_TYPE_t eEEPROMReadRegister( EEPROM_ADRESS_TYPE addr, void * pData );
void vGetRegToTime( uint8_t * DataStorage, PDM_DATA_TIME * data);
RECORD_DATA_TYPE_t eGetReocrdFieldsType(uint8_t index );
void vSetRegData(uint8_t * buf, uint8_t * data, uint8_t data_type);

EERPOM_ERROR_CODE_t eEEPROMWriteExternData ( uint32_t adr, const uint8_t* data, uint32_t length );
int eAccessToken( uint16_t token);
uint16_t usGetEEPROMSize();
uint16_t usEEPROMReadToExternData(EEPROM_ADRESS_TYPE addr, uint8_t * data, uint8_t len );
/* Функции быстрой записи непрерывной последовательности регистор
 *
 * Это API для случаев, когда нужно быстро скинуть массив данных в регистры.
 * Если записывать регистры стандарным API, то после каждой записи дрйвер будет жать. В слуай с быстрой записью, драйвер буфиризирует данные и записывает
 * их когда наберет данных на сектор.
 * Регистровую модель, нужно проектировать таким образом, чтобы регстры для операции быстрой записи
 * шли последовательно.
 * eEEPROMConfigFastWrite - конфигурация стартовго адреса
 * eEEPROMRegisterAddToFastWrite - запись в регистр. Если данные регистра попадают в сектро EEPROM, то данные сохрнаяются в буфере сктора,
 * если перелзают в другой сеткор, то формируетмя буффер сектора, он записывается в eeprom, а не записанная часть регистра сохраняется в буфере
 * eEEPROMRegisterFastWrtie(); - запись существущго буфера сектора в eeprom.

 * */
void eEEPROMConfigFastWrite(REGISTE_DATA_TYPE_t start_address);
void eEEPROMRegisterAddToFastWrite(void * data, REGISTE_DATA_TYPE_t datatype );
EERPOM_ERROR_CODE_t eEEPROMRegisterFastWrtie();



#endif /* DATASTORAGE_H_ */
