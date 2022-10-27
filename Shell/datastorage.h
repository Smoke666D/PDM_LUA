#ifndef DATASTORAGE_H_
#define DATASTORAGE_H_



#include "main.h"

#define Device_ADD 0x00A0
#define VALID_CODE   0x55
#define INVALID_CODE 0x00
#define WRITE_DATA_FRAME 0x06



typedef enum {
	EEPROM_OK,
	EEPROM_INIT_ERROR,
	EEPROM_NOT_VALIDE_ADRESS,
	EEPROM_READ_ERROR,
	EEPROM_WRITE_ERROR,
} EERPOM_ERROR_CODE_t;

typedef struct {
	uint8_t DATA[4];
	uint8_t VALID_FLAG;
} EEPROM_RECORD_t;

#define EEPROM_DATA_FRAME 5U
#define EEPROM_ADRESS_SIZE 1U
#define EEPROM_MAX_ADRRES 0x7FF
#define EEPROM_TIME_OUT 100U

#define GET_ADDR_MSB( ADDR) (( ADDR >>8U ) & 0x03)


void vEEPROMInit(I2C_HandleTypeDef * hi2c2);

#endif /* DATASTORAGE_H_ */
