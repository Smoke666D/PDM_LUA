/*
 * pdm_math.h
 *
 *  Created on: Nov 11, 2022
 *      Author: igor.dymov
 */

#ifndef PDM_MATH_H_
#define PDM_MATH_H_

#define PID_COUNT  12

#include "stm32f405xx.h"
#include "system.h"
#include  "main.h"
#include "arm_math.h"


void vInitPID(uint8_t pid_number, float32_t Kp,float32_t Ki,float32_t Kd);
void vProcessPID(uint8_t pid_number, float32_t input, float32_t * output);
void vPidReset(uint8_t pid_number);

#endif /* PDM_MATH_H_ */
