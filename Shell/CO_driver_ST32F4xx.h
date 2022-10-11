/*
 * CO_driver_ST32F103.h
 *
 *  Created on: Nov 17, 2021
 *      Author: igor.dymov
 */

#ifndef KEYPAD_CO_DRIVER_ST32F103_H_
#define KEYPAD_CO_DRIVER_ST32F103_H_

#include "main.h"
#include "cantask.h"
#include "cmsis_os.h"
//#include "CO_driver.h"
#include "CO_driver_target.h"





/* Wakeup specific threads */
#define CO_WAKEUP_APP_THREAD()                          osSemaphoreRelease(co_drv_app_thread_sync_semaphore)
#define CO_WAKEUP_PERIODIC_THREAD()                     osSemaphoreRelease(co_drv_periodic_thread_sync_semaphore)
#define CO_WAIT_SYNC_APP_THREAD(max_time_in_ms)         osSemaphoreAcquire(co_drv_app_thread_sync_semaphore, (max_time_in_ms))
#define CO_WAIT_SYNC_PERIODIC_THREAD(max_time_in_ms)    osSemaphoreAcquire(co_drv_periodic_thread_sync_semaphore, (max_time_in_ms))
/* (un)lock critical section in CO_CANsend() */

/**
 * Return values of some CANopen functions. If function was executed
 * successfully it returns 0 otherwise it returns <0.
 */
typedef enum {
    CO_ERROR_NO = 0,                /**< Operation completed successfully */
    CO_ERROR_ILLEGAL_ARGUMENT = -1, /**< Error in function arguments */
    CO_ERROR_OUT_OF_MEMORY = -2,    /**< Memory allocation failed */
    CO_ERROR_TIMEOUT = -3,          /**< Function timeout */
    CO_ERROR_ILLEGAL_BAUDRATE = -4, /**< Illegal baudrate passed to function
                                         CO_CANmodule_init() */
    CO_ERROR_RX_OVERFLOW = -5,      /**< Previous message was not processed
                                         yet */
    CO_ERROR_RX_PDO_OVERFLOW = -6,  /**< previous PDO was not processed yet */
    CO_ERROR_RX_MSG_LENGTH = -7,    /**< Wrong receive message length */
    CO_ERROR_RX_PDO_LENGTH = -8,    /**< Wrong receive PDO length */
    CO_ERROR_TX_OVERFLOW = -9,      /**< Previous message is still waiting,
                                         buffer full */
    CO_ERROR_TX_PDO_WINDOW = -10,   /**< Synchronous TPDO is outside window */
    CO_ERROR_TX_UNCONFIGURED = -11, /**< Transmit buffer was not configured
                                         properly */
    CO_ERROR_OD_PARAMETERS = -12,   /**< Error in Object Dictionary parameters*/
    CO_ERROR_DATA_CORRUPT = -13,    /**< Stored data are corrupt */
    CO_ERROR_CRC = -14,             /**< CRC does not match */
    CO_ERROR_TX_BUSY = -15,         /**< Sending rejected because driver is
                                         busy. Try again */
    CO_ERROR_WRONG_NMT_STATE = -16, /**< Command can't be processed in current
                                         state */
    CO_ERROR_SYSCALL = -17,         /**< Syscall failed */
    CO_ERROR_INVALID_STATE = -18,   /**< Driver not ready */
    CO_ERROR_NODE_ID_UNCONFIGURED_LSS = -19 /**< Node-id is in LSS unconfigured
                                         state. If objects are handled properly,
                                         this may not be an error. */
} CO_ReturnError_t;


/**
 * CAN error status bitmasks.
 *
 * CAN warning level is reached, if CAN transmit or receive error counter is
 * more or equal to 96. CAN passive level is reached, if counters are more or
 * equal to 128. Transmitter goes in error state 'bus off' if transmit error
 * counter is more or equal to 256.
 */
typedef enum {
    CO_CAN_ERRTX_WARNING = 0x0001,  /**< 0x0001, CAN transmitter warning */
    CO_CAN_ERRTX_PASSIVE = 0x0002,  /**< 0x0002, CAN transmitter passive */
    CO_CAN_ERRTX_BUS_OFF = 0x0004,  /**< 0x0004, CAN transmitter bus off */
    CO_CAN_ERRTX_OVERFLOW = 0x0008, /**< 0x0008, CAN transmitter overflow */

    CO_CAN_ERRTX_PDO_LATE = 0x0080, /**< 0x0080, TPDO is outside sync window */

    CO_CAN_ERRRX_WARNING = 0x0100,  /**< 0x0100, CAN receiver warning */
    CO_CAN_ERRRX_PASSIVE = 0x0200,  /**< 0x0200, CAN receiver passive */
    CO_CAN_ERRRX_OVERFLOW = 0x0800, /**< 0x0800, CAN receiver overflow */

    CO_CAN_ERR_WARN_PASSIVE = 0x0303/**< 0x0303, combination */
} CO_CAN_ERR_status_t;


CO_ReturnError_t CO_CANmodule_init(

        uint16_t                CANbitRate);

uint8_t uPDMGetCanReady();
void CO_CANsetNormalMode();
void CO_CANmodule_disable();
uint8_t  co_drv_create_os_objects(void);
uint8_t co_drv_mutex_lock(void);
uint8_t co_drv_mutex_unlock(void);
void CO_CANsetConfigurationMode();
uint8_t getCanFifoFree();
uint8_t uPDMCanSend(CAN_TX_FRAME_TYPE *buffer);
void vConfigCAN(CAN_HandleTypeDef  * pcan);
void HAL_CAN_TxMailbox0CompleteCallback(CAN_HandleTypeDef *hcan);
void HAL_CAN_TxMailbox1CompleteCallback(CAN_HandleTypeDef *hcan);
void HAL_CAN_TxMailbox2CompleteCallback(CAN_HandleTypeDef *hcan);
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan);
void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *hcan);

#endif /* KEYPAD_CO_DRIVER_ST32F103_H_ */
