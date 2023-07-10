/*
 * machineControl.h
 *
 *  Created on: Jul 9, 2023
 *      Author: harsha
 */

#ifndef MACHINECONTROL_H_
#define MACHINECONTROL_H_

#include "stdio.h"

#define CALENDER_ROLLER_CIRC 141.37
#define CALENDER_ROLLER_GEARBOX 6.91
#define LIFT_SCREW_PITCH_MM 4.0
#define LIFT_MOTOR_TO_LIFT_SCREW_RATIO 24

#define AVERAGE_BOBBIN_DIA 24
#define BINDWINDRATIO 2
#define CHASEHEIGHT 55

#define NE_TO_NM 1
#define NM_TO_NE 2

typedef struct machineSettings_Struct{
	int inputYarnCountNe;
	float outputYarnDia;
    int spindleSpeed;
    int tpi;
    int packageHeight;
    float diaBuildFactor;
    int windingOffsetCoils;
    int windingClosenessFactor;

}machineSettingsTypeDef;

typedef struct ringDoublerParameters{
	uint16_t spindleSpeed;
	float delivery_MtrMin;

	float req_CalRoller_RPM;
	uint16_t calenderMotorRPM;
	float calenderMotorRampRate; //for the 100ms target loop

	uint8_t outputYarnCountNe;
	float outputYarnDia_formula;

	float windingVelocity_mmSec;
	float bindingVelocity_mmSec;
	uint16_t windingPps;
	uint16_t bindingPps;

	float ht_difference_perStroke;
	uint16_t windingOffsetPulse;

	int strokesPerDoff;

	float windingStroke;
	float bindingStroke;
	uint16_t windingStrokePps;
	uint16_t bindingStrokePps;
	float windingTime;
	float bindingTime;

	float totalStrokeTime_min ;
	float doffTimeMin;

	float totalYarnLengthInMtrsPerBobbin;
	float outputYarnCountNm;
	float yarnWeightinGramsPerBobbin ;

	float maxBobbinDia;
}ringDoubler;

extern machineSettingsTypeDef ms;
extern ringDoubler rd;

float CalculateYarnDia(int yarnCount);
float ConvertYarnUnits(int input,int conversion);
void SetupRDCalculations(machineSettingsTypeDef *ms,ringDoubler *rd);
void ContinuousRDCalculations(machineSettingsTypeDef *ms,ringDoubler *rd);


#endif /* MACHINECONTROL_H_ */
