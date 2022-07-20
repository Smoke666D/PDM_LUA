/*
 * unity_config.h
 *
 *  Created on: 11 апр. 2022 г.
 *      Author: 79110
 */

#ifndef UNITY_CONFIG_H_
#define UNITY_CONFIG_H_

#include "common.h"

#define UNITY_EXCLUDE_SETJMP_H
#define UNITY_OUTPUT_CHAR( a )  vUNITputChar( a )
#define UNITY_OUTPUT_START()    vUNITresetOutput()
#define UNITY_OUTPUT_COMPLETE() vUNITwriteOutput()
#define UNITY_PRINT_EOL()       UNITY_OUTPUT_CHAR('\n');UNITY_OUTPUT_CHAR('\r')
#define UNITY_OUTPUT_COLOR
//UNITY_EXEC_TIME_START
//UNITY_EXEC_TIME_STOP
//UNITY_TIME_TYPE
//UNITY_PRINT_EXEC_TIME

#endif /* UNITY_CONFIG_H_ */
