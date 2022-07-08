/*
 * cantask.h
 *
 *  Created on: Jul 5, 2022
 *      Author: igor.dymov
 */

#ifndef CANTASK_H_
#define CANTASK_H_

#include "main.h"

typedef struct {
    uint16_t ident;
    uint8_t DLC;
    uint8_t data[8];
    uint8_t new_data;
} CANtx;

typedef struct {
	uint32_t ident;
	uint8_t DLC;
    uint8_t data[8];
} CAN_FRAME_TYPE;

uint8_t vCanChekMessage(uint32_t id);
uint8_t vCanGetMessage(CAN_FRAME_TYPE * RXPacket, uint8_t is_answer);
void SetMailboxFilter(uint32_t id);
void SetWaitFilter(uint32_t id);
void vCanTask(void *argument);
void vCanInsertTXData(uint32_t CanID, uint8_t * data, uint8_t data_len );
void vCanInsertToRXQueue(CAN_FRAME_TYPE * data);
#endif /* CANTASK_H_ */
