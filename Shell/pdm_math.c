/*
 * pdm_math.c
 *
 *  Created on: Nov 11, 2022
 *      Author: igor.dymov
 */


#include "pdm_math.h"



static arm_pid_instance_f32 PID[PID_COUNT]; //  __SECTION(RAM_SECTION_CCMRAM);

void vInitPID(uint8_t pid_number, float32_t Kp,float32_t Ki,float32_t Kd)
{
  if ( pid_number < PID_COUNT )
  {
	PID[pid_number].Kd = Kd;
	PID[pid_number].Ki = Ki;
	PID[pid_number].Kp = Kp;
	arm_pid_init_f32(&PID[pid_number], 0x01);
  }
  return;
}


float vProcessPID(uint8_t pid_number, float input)
{
     float res = 0;
	if ( pid_number < PID_COUNT )
	{
		res = arm_pid_f32(&PID[pid_number],input);
	}
	return ( res );
}

void vPidReset(uint8_t pid_number)
{
	if ( pid_number < PID_COUNT )
	{
		arm_pid_reset_f32(&PID[pid_number]);
	}
	return;
}
