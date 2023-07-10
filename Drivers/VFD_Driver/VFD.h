/*
 * VFD.h
 *
 *  Created on: Jun 7, 2023
 *      Author: harsha
 */

#ifndef VFD_DRIVER_VFD_H_
#define VFD_DRIVER_VFD_H_

#include "stdio.h"
#include "main.h"

#define SPINDLE_SPEED_6000 1
#define SPINDLE_SPEED_8000 2
#define SPINDLE_SPEED_10000 3
#define SPINDLE_SPEED_12000 4

#define CONTROL_LOOP_TIME_SEC 0.1

typedef struct VFDstruct{
	uint8_t VFD_setting;
	uint8_t vfdOn;

	uint16_t maxRPM;
	uint8_t rampTime_sec;
	float rampRate;
	int16_t presentRPM;
}VFD;

extern VFD vfd;

void VFD_setSpindleSpeed(VFD *v,uint8_t spindlespeed_define);
void VFD_startInductionMotor(VFD *v);
void VFD_stopInductionMotor(VFD *v);


#endif /* VFD_DRIVER_VFD_H_ */
