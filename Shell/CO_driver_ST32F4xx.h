/*
 * CO_driver_ST32F103.h
 *
 *  Created on: Nov 17, 2021
 *      Author: igor.dymov
 */

#ifndef KEYPAD_CO_DRIVER_ST32F103_H_
#define KEYPAD_CO_DRIVER_ST32F103_H_

#include "main.h"
#include "cmsis_os.h"
#include "CO_driver.h"
#include "CO_driver_target.h"

#define MAILBOXSIZE  14*4



/* Wakeup specific threads */
#define CO_WAKEUP_APP_THREAD()                          osSemaphoreRelease(co_drv_app_thread_sync_semaphore)
#define CO_WAKEUP_PERIODIC_THREAD()                     osSemaphoreRelease(co_drv_periodic_thread_sync_semaphore)
#define CO_WAIT_SYNC_APP_THREAD(max_time_in_ms)         osSemaphoreAcquire(co_drv_app_thread_sync_semaphore, (max_time_in_ms))
#define CO_WAIT_SYNC_PERIODIC_THREAD(max_time_in_ms)    osSemaphoreAcquire(co_drv_periodic_thread_sync_semaphore, (max_time_in_ms))
/* (un)lock critical section in CO_CANsend() */



CO_ReturnError_t CO_CANmodule_init(

        uint16_t                CANbitRate);


uint8_t  co_drv_create_os_objects(void);
uint8_t co_drv_mutex_lock(void);
uint8_t co_drv_mutex_unlock(void);
void CO_CANsetConfigurationMode();
uint8_t getCanFifoFree();
uint8_t uPDMCanSend(CO_CANtx_t *buffer);
void vConfigCAN(CAN_HandleTypeDef  * pcan);
void HAL_CAN_TxMailbox0CompleteCallback(CAN_HandleTypeDef *hcan);
void HAL_CAN_TxMailbox1CompleteCallback(CAN_HandleTypeDef *hcan);
void HAL_CAN_TxMailbox2CompleteCallback(CAN_HandleTypeDef *hcan);
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan);
void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *hcan);

#endif /* KEYPAD_CO_DRIVER_ST32F103_H_ */
