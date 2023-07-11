/*
 * VFD.c
 *
 *  Created on: Jun 7, 2023
 *      Author: harsha
 */

//VFD Architecture
//MOT1 and MOT2 are the two calender rollers.
//steppers have their own connectors. In pinili we use the MOT7 and MOT8 to control the steppers.
//MOT3,4,5,6 are used to control the photocouplers to the VFD

//The Run pin in the VFD is connected to OUT3 here in the uC
//MI1 is on OUT4, MI2 is on OUT5 and MI3 is on OUT6


#include "VFD.h"


void VFD_setSpindleSpeed(VFD *v,uint8_t spindlespeed_define){
	v->VFD_setting = spindlespeed_define;
	if (v->VFD_setting == SPINDLE_SPEED_6000){
		v->rampTime_sec = 30;
		v->maxRPM = 6000;
	}
	if (v->VFD_setting == SPINDLE_SPEED_8000){
		v->rampTime_sec = 40;
		v->maxRPM = 8000;
	}
	if (v->VFD_setting == SPINDLE_SPEED_9000){
		v->rampTime_sec = 45;
		v->maxRPM = 9000;
	}
	if (v->VFD_setting == SPINDLE_SPEED_10000){
		v->rampTime_sec = 50;
		v->maxRPM = 10000;
	}
	v->rampRate = v->maxRPM/(v->rampTime_sec *10);
}

void VFD_startInductionMotor(VFD *v){
	if (v->VFD_setting == SPINDLE_SPEED_6000){
		HAL_GPIO_WritePin(OUT3_GPIO_Port, OUT3_Pin, GPIO_PIN_SET); // Start command
		HAL_GPIO_WritePin(OUT4_GPIO_Port, OUT4_Pin, GPIO_PIN_SET); //MI1
 		HAL_GPIO_WritePin(OUT5_GPIO_Port, OUT5_Pin, GPIO_PIN_RESET); //MI2
		HAL_GPIO_WritePin(OUT6_GPIO_Port, OUT6_Pin, GPIO_PIN_RESET); //MI3
	}else if (v->VFD_setting == SPINDLE_SPEED_8000){
		HAL_GPIO_WritePin(OUT3_GPIO_Port, OUT3_Pin, GPIO_PIN_SET); // Start command
		HAL_GPIO_WritePin(OUT4_GPIO_Port, OUT4_Pin, GPIO_PIN_RESET); 	//MI1
		HAL_GPIO_WritePin(OUT5_GPIO_Port, OUT5_Pin, GPIO_PIN_SET); //MI2
		HAL_GPIO_WritePin(OUT6_GPIO_Port, OUT6_Pin, GPIO_PIN_RESET); //MI3
	}else if (v->VFD_setting == SPINDLE_SPEED_9000){
		HAL_GPIO_WritePin(OUT3_GPIO_Port, OUT3_Pin, GPIO_PIN_SET); // Start command
		HAL_GPIO_WritePin(OUT4_GPIO_Port, OUT4_Pin, GPIO_PIN_SET); //MI1
		HAL_GPIO_WritePin(OUT5_GPIO_Port, OUT5_Pin, GPIO_PIN_SET); //MI2
		HAL_GPIO_WritePin(OUT6_GPIO_Port, OUT6_Pin, GPIO_PIN_RESET); //MI3
	}
	else if (v->VFD_setting == SPINDLE_SPEED_10000){
		HAL_GPIO_WritePin(OUT3_GPIO_Port, OUT3_Pin, GPIO_PIN_SET); // Start command
		HAL_GPIO_WritePin(OUT4_GPIO_Port, OUT4_Pin, GPIO_PIN_RESET); 	//MI1
		HAL_GPIO_WritePin(OUT5_GPIO_Port, OUT5_Pin, GPIO_PIN_RESET); //MI2
		HAL_GPIO_WritePin(OUT6_GPIO_Port, OUT6_Pin, GPIO_PIN_SET); //MI3
	}else{

	}
	v->vfdOn = 1;
}

void VFD_stopInductionMotor(VFD *v){
	HAL_GPIO_WritePin(OUT3_GPIO_Port, OUT3_Pin, GPIO_PIN_RESET); // Start command
	HAL_GPIO_WritePin(OUT4_GPIO_Port, OUT4_Pin, GPIO_PIN_RESET); 	//MI1
	HAL_GPIO_WritePin(OUT5_GPIO_Port, OUT5_Pin, GPIO_PIN_RESET); //MI2
	HAL_GPIO_WritePin(OUT6_GPIO_Port, OUT6_Pin, GPIO_PIN_RESET); //MI3
	v->vfdOn = 0;
}
