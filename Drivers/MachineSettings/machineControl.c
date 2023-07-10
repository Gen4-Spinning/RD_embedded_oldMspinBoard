/*
 * machineControl.c
 *
 *  Created on: Jul 9, 2023
 *      Author: harsha
 */

#include "machineControl.h"
#include "VFD.h"
#include "math.h"

float CalculateYarnDia(int yarnCount){
	return -0.10284 + 1.592/sqrt((double)yarnCount);  //from the link : https://textilelearner.net/yarn-count-diameter-and-composition/
}

float ConvertYarnUnits(int input,int conversion){
	float output = 0;
	if (conversion == NE_TO_NM){
		output = input / 0.591;
	}else if (conversion == NM_TO_NE){
		output = input * 0.591;
	}
	return output;
}


void ContinuousRDCalculations(machineSettingsTypeDef *ms,ringDoubler *rd){
	rd->spindleSpeed = vfd.presentRPM;

	//some of these calculations only need to be made till the spindle speed max is reached.
	rd->delivery_MtrMin = ((float)(rd->spindleSpeed)/ms->tpi) * 0.0254;

	rd->req_CalRoller_RPM = (rd->delivery_MtrMin*1000)/CALENDER_ROLLER_CIRC;
	rd->calenderMotorRPM = rd->req_CalRoller_RPM * CALENDER_ROLLER_GEARBOX;

	float windingClosenessFactor_float = ms->windingClosenessFactor/100.0;
	float temp1 = ((rd->delivery_MtrMin*1000)/(3.14 * AVERAGE_BOBBIN_DIA ));
	float temp2 = ms->outputYarnDia* windingClosenessFactor_float;

	rd->windingVelocity_mmSec = (temp1 * temp2 )/60; //in mm/sec
	rd->bindingVelocity_mmSec = rd->windingVelocity_mmSec * BINDWINDRATIO;

	rd->windingPps = (100*rd->windingVelocity_mmSec);
	rd->bindingPps = (100*rd->bindingVelocity_mmSec);
}

void SetupRDCalculations(machineSettingsTypeDef *ms,ringDoubler *rd){
	rd->spindleSpeed = ms->spindleSpeed;

	//some of these calculations only need to be made till the spindle speed max is reached.
	rd->delivery_MtrMin = ((float)(rd->spindleSpeed)/ms->tpi) * 0.0254;

	rd->req_CalRoller_RPM = (rd->delivery_MtrMin*1000)/CALENDER_ROLLER_CIRC;
	rd->calenderMotorRPM = rd->req_CalRoller_RPM * CALENDER_ROLLER_GEARBOX;
	rd->calenderMotorRampRate = rd->calenderMotorRPM/(vfd.rampTime_sec * 10);

	float windingClosenessFactor_float = ms->windingClosenessFactor/100.0;
	float temp1 = ((rd->delivery_MtrMin*1000)/(3.14 * AVERAGE_BOBBIN_DIA ));
	float temp2 = ms->outputYarnDia* windingClosenessFactor_float;

	rd->windingVelocity_mmSec = (temp1 * temp2 )/60; //in mm/sec
	rd->bindingVelocity_mmSec = rd->windingVelocity_mmSec * BINDWINDRATIO;

	rd->windingPps = (100*rd->windingVelocity_mmSec);
	rd->bindingPps = (100*rd->bindingVelocity_mmSec);

	rd->outputYarnCountNe = ms->inputYarnCountNe/2;
	rd->outputYarnDia_formula = CalculateYarnDia(rd->outputYarnCountNe);
	rd->outputYarnCountNm = ConvertYarnUnits(rd->outputYarnCountNe,NE_TO_NM);

	//WE calculate all these params, but we use the dia set by the user
	rd->ht_difference_perStroke = (ms->windingOffsetCoils * ms->outputYarnDia * windingClosenessFactor_float)/ms->diaBuildFactor;
	rd->windingOffsetPulse = rd->ht_difference_perStroke * 100;

	//needed to check if length has been reached
	rd->windingStrokePps = (CHASEHEIGHT*100);
	rd->bindingStrokePps = rd->windingStrokePps - rd->windingOffsetPulse;

	rd->strokesPerDoff = (ms->packageHeight - CHASEHEIGHT)/rd->ht_difference_perStroke;

	rd->windingStroke = CHASEHEIGHT;
	rd->bindingStroke = CHASEHEIGHT - rd->ht_difference_perStroke;
	rd->windingTime = CHASEHEIGHT/rd->windingVelocity_mmSec;
	rd->bindingTime = rd->bindingStroke/rd->bindingVelocity_mmSec;

	rd->totalStrokeTime_min = (rd->windingTime + rd->bindingTime)/60;
	rd->doffTimeMin =  rd->strokesPerDoff*rd->totalStrokeTime_min;

	rd->totalYarnLengthInMtrsPerBobbin = rd->doffTimeMin * rd->delivery_MtrMin;
	rd->yarnWeightinGramsPerBobbin = rd->totalYarnLengthInMtrsPerBobbin/rd->outputYarnCountNm;

	rd->maxBobbinDia = rd->strokesPerDoff * 2 * windingClosenessFactor_float * ms->outputYarnDia + AVERAGE_BOBBIN_DIA;

}
