/*
 * cantask.h
 *
 *  Created on: Jul 5, 2022
 *      Author: igor.dymov
 */

#ifndef CANTASK_H_
#define CANTASK_H_

#include "main.h"
#include "cmsis_os.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#define MAILBOXSIZE  14*4

/* Transmit message object */
typedef struct {
    uint32_t ident;
    uint8_t DLC;
    uint8_t data[8];
} CAN_TX_FRAME_TYPE;

typedef struct {
    uint16_t ident;
    uint8_t DLC;
    uint8_t data[8];
    uint16_t filter_id;
    uint8_t new_data;
} CANRX;

typedef struct {
	uint32_t ident;
	uint8_t DLC;
    uint8_t data[8];
    uint16_t filter_id;
} CAN_FRAME_TYPE;

QueueHandle_t* pCANRXgetQueue ( void );
QueueHandle_t* pCANTXgetQueue ( void );
uint8_t CheckAnswer( void );
uint8_t vCanGetRequest(CAN_FRAME_TYPE * RXPacket);
uint8_t vCanChekMessage(uint32_t id);
uint8_t vCanGetMessage(CAN_FRAME_TYPE * RXPacket);
void SetMailboxFilter(uint32_t id);
void SetWaitFilter(uint32_t id);
void vCanTask(void *argument);
void vCanInsertTXData(uint32_t CanID, uint8_t * data, uint8_t data_len );
void vCanInsertToRXQueue(CAN_FRAME_TYPE * data);
#endif /* CANTASK_H_ */
