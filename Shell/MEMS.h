/*
 * MEMS.h
 *
 *  Created on: Nov 11, 2022
 *      Author: igor.dymov
 */

#ifndef MEMS_H_
#define MEMS_H_

#define ALGO_FREQ  100U
#define DECIMATION  4U

void vmemsTask(void *argument);
void MX_DynamicInclinometer_Process();

#endif /* MEMS_H_ */
