/*
 * EEPROM.h
 *
 *  Created on: Oct 26, 2022
 *      Author: igor.dymov
 */

#ifndef EEPROM_H_
#define EEPROM_H_

#include "main.h"


#define Device_ADD 0x00A0

void vEEPROMInit(I2C_HandleTypeDef * hi2c2);

#endif /* EEPROM_H_ */
