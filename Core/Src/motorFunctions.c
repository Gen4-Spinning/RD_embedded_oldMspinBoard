
//#include "MotorStruct.h"
#include "main.h"
#include "stm32f4xx_hal.h"

#include "logicDefines.h"
#include "Initialize.h"
#include "functionDefines.h"
#include "Structs.h"
#include "CommonConstants.h"
#include "HMI_Constants.h"
#include "HMI_Fns.h"
#include <stdlib.h>
#include "math.h"
#include "../../Drivers/Encoder/encoder.h"
#include "VFD.h"

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;

extern int filter1;
extern int startFlag;
extern int currentLayer;

extern char MOTORARRAY[];
extern char MOTORARRAY_HMI[];

int errorCount = 0;
int errorCount1 = 0;

extern int bobbin_DC_Offset;
extern int backRoller_DC_Offset ;

extern int pulseCount1;
extern int pulseCount;

extern unsigned int chaseCount;

void AllTimerOn(void)
{
		MotorTimer(MOTOR1_TIMER_ON);
		MotorTimer(MOTOR2_TIMER_ON);
		MotorTimer(MOTOR3_TIMER_ON);
		MotorTimer(MOTOR4_TIMER_ON);
		MotorTimer(MOTOR5_TIMER_ON);
		MotorTimer(MOTOR6_TIMER_ON);
		MotorTimer(MOTOR7_TIMER_ON);
}


void ResetMotorVariables(uint8_t motorIndex){
	M[motorIndex].intTarget = 0;
	M[motorIndex].integralError = 0;
	M[motorIndex].overloadCount = 0;
	M[motorIndex].antiWindup = 0;
}

/********************************************StepMotorPID1 function START*************************************************/
void StepMotorPID(int motorIndex)
{
	unsigned int calcPwm = 0;
	
	M[motorIndex].presentRpm = GetRPM(&encs,motorIndex);
	M[motorIndex].intTarget = M[motorIndex].setRpm;

	M[motorIndex].error = M[motorIndex].intTarget - M[motorIndex].presentRpm;
//	M[motorIndex].error = M[motorIndex].intTarget - filter1;
	
	
	/*if (__fabs(M[motorIndex].error) >= 300)
	{
		errorCount++;
		if(errorCount >= 20)
		{
		E.RpmErrorFlag = 1;
		S.errStopReason = MOTORARRAY[motorIndex];
		S.errStopReasonHMI = MOTORARRAY_HMI[motorIndex];
		S.errmotorFault = ERR_RPM_ERROR;	
		S.errVal = NO_FLOAT_VAR;	
		errorCount = 0;
		}
	}*/
	
	M[motorIndex].integralError = (M[motorIndex].integralError + M[motorIndex].error);
	calcPwm = M[motorIndex].Kp*M[motorIndex].error + M[motorIndex].Ki*M[motorIndex].integralError;
	
	if ((calcPwm <= MAX_PWM) && (calcPwm > 0))
	{
		M[motorIndex].pwm = calcPwm;
	}
	if(calcPwm <= 0)
	{
		M[motorIndex].pwm = 0;
	}
}


/********************************************StepMotorPID function START************************************/

void UpdateMotorPID_w_DCOffset(int motorIndex,int dc_offset)
{
	unsigned int calcPwm = 0;
	M[motorIndex].presentRpm = GetRPM(&encs,motorIndex);
	
	if((startFlag == 1) && ((M[motorIndex].intTarget <= M[motorIndex].setRpm)))
	{
		M[motorIndex].intTarget = M[motorIndex].intTarget + M[motorIndex].rampRpm;
	}
	
	if((startFlag == 1) && ((M[motorIndex].intTarget > M[motorIndex].setRpm)))
	{
		M[motorIndex].intTarget = M[motorIndex].intTarget - M[motorIndex].rampRpm;
	}
	
	if((startFlag == 0)&&((M[motorIndex].intTarget > 0)))
	{
		M[motorIndex].intTarget = M[motorIndex].intTarget - M[motorIndex].rampRpm;
	}
	
	M[motorIndex].error = M[motorIndex].intTarget - M[motorIndex].presentRpm;
//	M[motorIndex].error = M[motorIndex].intTarget - filter1;
	
	if (abs(M[motorIndex].error) >= 400)
	{
		errorCount1++;
	if(errorCount1 >= 10)
		{
		E.RpmErrorFlag = 1;
		S.errStopReason = MOTORARRAY[motorIndex];
		S.errStopReasonHMI = MOTORARRAY_HMI[motorIndex];
		S.errmotorFault = ERR_RPM_ERROR;	
		S.errVal = NO_FLOAT_VAR;	
		errorCount1 = 0;
		}
	}
	
	M[motorIndex].integralError = (M[motorIndex].integralError + M[motorIndex].error);
	calcPwm = M[motorIndex].Kp*M[motorIndex].error + M[motorIndex].Ki*M[motorIndex].integralError;
	calcPwm = calcPwm  + dc_offset;

	
	if ((calcPwm <= MAX_PWM) && (calcPwm > dc_offset))
	{
		M[motorIndex].pwm = calcPwm;
	}
	if((calcPwm <= dc_offset) && (calcPwm > 0 ))
	{
		M[motorIndex].pwm = dc_offset;
	}
}

void StepMotorPID_w_DCOffset(int motorIndex,int offset)
{
	unsigned int calcPwm = 0;

	M[motorIndex].presentRpm = GetRPM(&encs,motorIndex);
	M[motorIndex].intTarget = M[motorIndex].setRpm;

	
	M[motorIndex].error = M[motorIndex].intTarget - M[motorIndex].presentRpm;
	
	if (abs(M[motorIndex].error) >= 200)
	{
		errorCount++;
		if(errorCount >= 10)
		{
		E.RpmErrorFlag = 1;
		S.errStopReason = MOTORARRAY[motorIndex];
		S.errStopReasonHMI = MOTORARRAY_HMI[motorIndex];
		S.errmotorFault = ERR_RPM_ERROR;	
		S.errVal = NO_FLOAT_VAR;	
		errorCount = 0;
		}
	}
	
	M[motorIndex].integralError = (M[motorIndex].integralError + M[motorIndex].error);
	calcPwm = M[motorIndex].Kp*M[motorIndex].error + M[motorIndex].Ki*M[motorIndex].integralError;
	calcPwm = calcPwm + offset;

	if ((calcPwm <= MAX_PWM) && (calcPwm > offset))
	{
		M[motorIndex].pwm = calcPwm;
	}
	if(calcPwm <= offset)
	{
		M[motorIndex].pwm = offset;
	}
}


void UpdateMotorPID(int motorIndex)
{
	unsigned int calcPwm = 0;
	M[motorIndex].presentRpm = GetRPM(&encs,motorIndex);
	
	if((startFlag == 1) && ((M[motorIndex].intTarget <= M[motorIndex].setRpm)))
	{
		M[motorIndex].intTarget = M[motorIndex].intTarget + 5;
	}
	
	if((startFlag == 1) && ((M[motorIndex].intTarget > M[motorIndex].setRpm)))
	{
		M[motorIndex].intTarget = M[motorIndex].intTarget - 5;
	}
	
	if((startFlag == 0)&&((M[motorIndex].intTarget > 0)))
	{
		M[motorIndex].intTarget = M[motorIndex].intTarget - 25;
	}
	
	M[motorIndex].error = M[motorIndex].intTarget - M[motorIndex].presentRpm;
	
	/*if (__fabs(M[motorIndex].error) >= 300)
	{
		E.RpmErrorFlag = 1;
		S.errStopReason = MOTORARRAY[motorIndex];
		S.errStopReasonHMI = MOTORARRAY_HMI[motorIndex];
		S.errmotorFault = ERR_RPM_ERROR;	
		S.errVal = NO_FLOAT_VAR;	
	}*/
	
	M[motorIndex].integralError = (M[motorIndex].integralError + M[motorIndex].error);
	calcPwm = M[motorIndex].Kp*M[motorIndex].error + M[motorIndex].Ki*M[motorIndex].integralError;
	if (calcPwm <= MAX_PWM)
	{
		M[motorIndex].pwm = calcPwm;
	}
}

char CheckAllMotors(void)
{
	char motorOnSignal = 0;
	if (M[MOTOR1].presentRpm >= (M[MOTOR1].setRpm - 100))
	{
		motorOnSignal = 1;
	}
	return motorOnSignal;
}


char RpmError(void){
	char out = 0;
	if(E.RpmErrorFlag == 1){
		out = 1;
		MotorDrive(DISABLE_D);
	}
	return out;
}


void MotorTimer(char index)
{
		switch(index)
	{
		case MOTOR1_TIMER_ON:
				HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_4);
		break;
		
		case MOTOR1_TIMER_OFF:
				HAL_TIM_PWM_Stop(&htim2,TIM_CHANNEL_4);
		break;
		
		case MOTOR2_TIMER_ON:
				HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_3);
		break;
		
		case MOTOR2_TIMER_OFF:
				HAL_TIM_PWM_Stop(&htim2,TIM_CHANNEL_3);
		break;
		
		case MOTOR3_TIMER_ON:
		//		HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_1);
		break;
		
		case MOTOR3_TIMER_OFF:
		//		HAL_TIM_PWM_Stop(&htim1,TIM_CHANNEL_1);
		break;
		
		case MOTOR4_TIMER_ON:
		//		HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_2);
		break;
		
		case MOTOR4_TIMER_OFF:
		//		HAL_TIM_PWM_Stop(&htim1,TIM_CHANNEL_2);
		break;
		
		case MOTOR5_TIMER_ON:
		//		HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_3);
		break;
		
		case MOTOR5_TIMER_OFF:
		//		HAL_TIM_PWM_Stop(&htim1,TIM_CHANNEL_3);
		break;
		
		case MOTOR6_TIMER_ON:
		//		HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_4);
		break;
		
		case MOTOR6_TIMER_OFF:
		//		HAL_TIM_PWM_Stop(&htim1,TIM_CHANNEL_4);
		break;
		
		case MOTOR7_TIMER_ON:
				HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_4);
		break;
		
		case MOTOR7_TIMER_OFF:
				HAL_TIM_PWM_Stop(&htim3,TIM_CHANNEL_4);
		break;
	}
}

void PauseMachine(void)
{
	int i = 0;
	HAL_Delay(100);
	for(i=1;i<6;i++)
		{
			M[i].pwm = 0;
		}
}

void ResetSecondaryMotor(void){
	M[MOTOR1].pwm = 0	;
	M[MOTOR2].pwm = 0;

	M[MOTOR1].intTarget = 0;
	M[MOTOR2].intTarget = 0;

	M[MOTOR1].error = 0;
	M[MOTOR2].error = 0;
	
	M[MOTOR1].integralError = 0;
	M[MOTOR2].integralError = 0;

}




unsigned int frontMotorRpm1 = 0;
unsigned int backMotorRpm1 = 0;
unsigned int spindleMotorRpm = 0;
unsigned int windingPps1 = 0;
unsigned int bindingPps1 = 0;
unsigned int windingLength1 = 0;
unsigned int bindingLength1 = 0;
float yarnDia = 0;
float deliverySpeed = 0;
float windingCloseness = 0;
float windingMM = 0;
unsigned int spindleSpeedConv = 0;

float shortFall = 0;
float yarnLength = 0;
float doffTime = 0;
float strokesPerDoff = 0;
float timePerStroke = 0;
float deliverySpeedMmin = 0;
float doffTimeMin = 0;
float delivSpeedMinmm = 0;
float yarnContent = 0;

void calculateRpm2(void){
	windingCloseness = 100;//((fsp.windingCloseness/100.0)+1.0);
	spindleSpeedConv = ceil(M[MOTOR1].intTarget*5.029703);
	frontMotorRpm1 = ceil((spindleSpeedConv/fsp.tpi)*4.49106);
	backMotorRpm1 = ceil((frontMotorRpm1/fsp.draft)*16.0);
	
	yarnDia = (1.592/(pow(fsp.yarnCount,0.5)));
	deliverySpeed = (spindleSpeedConv/fsp.tpi)*25.4;
	windingMM = deliverySpeed/78.5398*windingCloseness*yarnDia;
	windingPps1 = (100*(windingMM/60));
	bindingPps1 = ceil(windingPps1*fsp.bindWindRatio);
	
	windingLength1 = (fsp.chaseLength*100);
	bindingLength1 = (fsp.chaseLength - 100);//fsp.windingOffset)*100;
	
	deliverySpeedMmin = deliverySpeed/1000.0;
	shortFall = ((10.0/100.0)+1.0)-1.0;  //hardcore value for now
	
	yarnContent = 73.5; //73.468*(fsp.chaseCount/100.0);
	yarnLength = yarnContent*fsp.yarnCount/0.6;
	
	//delivSpeedMinmm = fsp.spindleSpeed/fsp.tpi * 25.4/1000.0;
	doffTime = yarnLength/deliverySpeedMmin;
	doffTimeMin = doffTime-(doffTime*shortFall);
	timePerStroke = fsp.chaseLength*(1/windingMM)*(1+1/fsp.bindWindRatio);
	strokesPerDoff = doffTime/timePerStroke;
	
//	M[MOTOR1].setRpm = spindleMotorRpm;
	M[MOTOR2].setRpm = frontMotorRpm1;
	M[MOTOR3].setRpm = backMotorRpm1;
	M[MOTOR4].setRpm = frontMotorRpm1;
	M[MOTOR5].setRpm = backMotorRpm1;
}


/*
float desiredContent = 0;
int windingOffsetPulse = 0;
float frontMotorRpmFloat = 0;
float spindleSpeedConvFloat = 0;
void calculateRpm3(void){
	windingCloseness = 1.08;
	spindleSpeedConvFloat = vfd.presentRPM;
	spindleSpeedConv = (int)spindleSpeedConvFloat;

	frontMotorRpmFloat = (spindleSpeedConvFloat/fsp.tpi)*1.347512;  //changed from 4.177
	frontMotorRpm1 = (int)frontMotorRpmFloat;

//	backMotorRpm1 = ceil((frontMotorRpmFloat/fsp.draft)*15.5);

	yarnDia = (1.592/(pow((fsp.yarnCount),0.5)));
	deliverySpeed = (spindleSpeedConv/fsp.tpi)*25.4;

	windingMM = (deliverySpeed/78.5398)*windingCloseness*yarnDia;
	windingPps1 = (100*(windingMM/60));
	bindingPps1 = ceil(windingPps1*fsp.bindWindRatio);

	deliverySpeedMmin = deliverySpeed/1000.0;
	shortFall = 0.1; //hardcore value for now

	yarnContent = 100; // // was 107;
	desiredContent = yarnContent * (fsp.preferredPackageSize/100.0);
	yarnLength = desiredContent*(fsp.yarnCount)/0.6;

	doffTime = yarnLength/deliverySpeedMmin;
	timePerStroke = fsp.chaseLength*(1/windingMM)*(1+1/fsp.bindWindRatio);
	strokesPerDoff = doffTime/timePerStroke;

	windingOffsetPulse = ceil((((7.68 * 25.4) - fsp.chaseLength)/strokesPerDoff) * 100);

	windingLength1 = (fsp.chaseLength*100);
	bindingLength1 = windingLength1 - windingOffsetPulse;

	M[MOTOR2].setRpm = frontMotorRpm1;
	M[MOTOR3].setRpm = backMotorRpm1;
	M[MOTOR4].setRpm = frontMotorRpm1;
	M[MOTOR5].setRpm = backMotorRpm1;
}
*/

char MaxLayerComplete(void)
{
	char out = 0;
	//char errorMotor = 0;
	if(chaseCount >= (strokesPerDoff*2))
	{
		out = 1;
	}

	return out;
}

// check max limit
char LiftLimitCheck(void)
{
	char out = 0;
	char sensorStatus1 = 0;
	//char sensorStatus2 = 0;
	sensorStatus1= InputSensor1();
//	sensorStatus2 = InputSensor2();
//	if((sensorStatus1 == 1) || (sensorStatus2 ==1))
	if(sensorStatus1 == 1)
	{
		out = 1;
	}
	return out;
}









