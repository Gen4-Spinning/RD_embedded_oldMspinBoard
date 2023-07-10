#ifndef __ENCODER_H
#define __ENCODER_H

#include "stdio.h"

#define TO_RPM_50MS_768_PPR 1.5625 // RPM = counts*(20*60/768)

typedef struct MotorEncoder{
	uint16_t rawCount;
	uint16_t averagedCount;
	uint16_t summedCount;
	float RPM;
}encMotor;

typedef struct enc{
	encMotor leftFR;
	encMotor rightFR;
	uint8_t averagingStarted;
	uint8_t averagingIDX;
}encoders;

extern encoders encs;


void UpdateRPM(encoders *enc);
uint16_t GetRPM(encoders *enc,uint8_t index);
void ResetEncoderVariables(encoders *enc);


#endif /* __ENCODER_H */

