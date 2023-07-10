
#include "encoder.h"
#include "logicDefines.h"
#include "functionDefines.h"

extern int pulseCount;


void UpdateRPM(encoders *enc){
	/* if not 12 readings, just take the raw reading as the averaged reading and keep summing the count.
	 * else the subtract the average from the summed count, and then add the new value for a new summed count
	 * and then divide by 12 for averaged count for the next reading.
	 */
	if ((enc->averagingIDX < 12) && (enc->averagingStarted == 0)){
		enc->leftFR.averagedCount = enc->leftFR.rawCount;
		enc->leftFR.summedCount += enc->leftFR.rawCount;

		enc->rightFR.averagedCount = enc->rightFR.rawCount;
		enc->rightFR.summedCount += enc->rightFR.rawCount;

		enc->averagingStarted = 1;
	}else{
		enc->leftFR.summedCount = enc->leftFR.summedCount  - enc->leftFR.averagedCount + enc->leftFR.rawCount;
		enc->leftFR.averagedCount = enc->leftFR.summedCount/12;

		enc->rightFR.summedCount = enc->rightFR.summedCount  - enc->rightFR.averagedCount + enc->rightFR.rawCount;
		enc->rightFR.averagedCount = enc->rightFR.summedCount/12;
	}

	enc->averagingIDX ++;


	enc->leftFR.RPM = (uint16_t)(enc->leftFR.averagedCount * TO_RPM_50MS_768_PPR);
	enc->leftFR.rawCount = 0;

	enc->rightFR.RPM = (uint16_t)(enc->rightFR.averagedCount* TO_RPM_50MS_768_PPR);
	enc->rightFR.rawCount = 0;
}


uint16_t GetRPM(encoders *enc,uint8_t index){
	if (index == MOTOR1){
		return ((uint16_t)enc->leftFR.RPM);
	}
	if (index == MOTOR2){
		return ((uint16_t)enc->rightFR.RPM);
	}
	return 0;
}


void ResetEncoderVariables(encoders *enc){
	enc->averagingIDX = 0;
	enc->averagingStarted = 0;

	enc->leftFR.summedCount = 0;
	enc->leftFR.averagedCount = 0;
	enc->leftFR.RPM = 0;
	enc->leftFR.rawCount = 0;

	enc->rightFR.summedCount = 0;
	enc->rightFR.averagedCount = 0;
	enc->rightFR.RPM = 0;
	enc->rightFR.rawCount = 0;
}













