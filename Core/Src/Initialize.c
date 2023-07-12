#include "Structs.h"
#include "CommonConstants.h"
#include "Initialize.h"
#include "logicDefines.h"
#include "HMI_Constants.h"
#include <math.h>
//Load the structs here and make them extern in the file you want.

extern int currentLayer;
extern int rampRpm;

void InitializeStateStruct(){
	S.current_state = IDLE_STATE;
	S.prev_state = OFF_STATE;
	S.paired = 0;
	S.state_change = 0;
	S.first_enter = 1;
	S.oneTimeflag = 0;
	S.errStopReason = NO_VAR;
	S.errStopReasonHMI = NO_VAR;
	S.errmotorFault = NO_VAR;
	S.errVal = NO_VAR;
	S.firstSwitchon = 1;
	S.runMode = HMI_RUN_RAMPUP; 
	S.changeRunMode = 0;
	S.RPI_idleSettingsTransferOnce = 0;
	S.oneSecTimer = 0;
	S.displayRTF = 0;
	S.runningRTF = 0;
	S.keyState = ENABLE_KEYPRESS;
	S.currentStateSaved = 0;
	S.doffState = CONTINUE_OLD_DOFF;
	S.savedStateExists = 0;
}


void InitializeUartStruct()
{
	U.TXcomplete =0; //needs to be initialized to start at 1!!
	U.TXtransfer =0;
	U.RPI_TXcomplete = 0;
}
void InitializeDiagnosticsStruct(){
	D.motorID = 0;
	D.motorID = 0;
	D.targetSignal=0;
	D.testTime = 0;
	D.typeofTest = 0;
	D.liftDirection = 0;
	D.liftDistance = 0;
	D.liftMode = 0;
	D.liftOver = 0;
	D.liftMotor = 0;
}

void MotorStructInit(void)	{
	M[MOTOR1].presentRpm = 0;
	M[MOTOR1].setRpm = ceil((fsp.spindleSpeed/10.92)*3.42);
	M[MOTOR1].rampRpm = 5; //was 10
	M[MOTOR1].piecingRpm = 300;
	M[MOTOR1].error = 0;
	M[MOTOR1].integralError = 0;
	M[MOTOR1].feedforward = 0;
	M[MOTOR1].ff_multiplier = 0.5f;//0
	M[MOTOR1].intTarget = 0;
	M[MOTOR1].calcPwm = 0;
	M[MOTOR1].pwm = 0;
	M[MOTOR1].Ki = 0.1;
	M[MOTOR1].Kp = 1.0;
	M[MOTOR1].startOffset = 150;
	M[MOTOR1].overloadDelta = 300; // delta rpm of 300
	M[MOTOR1].overloadTime = 50; // for 5 sec at 0.1 sec interrupt
	M[MOTOR1].overloadCount = 0;
	M[MOTOR1].antiWindup = 0;


	M[MOTOR2].presentRpm = 0;
	M[MOTOR2].setRpm = ceil((fsp.spindleSpeed/10.92)*3.42);
	M[MOTOR2].rampRpm = 5; //was 10
	M[MOTOR2].piecingRpm = 300;
	M[MOTOR2].error = 0;
	M[MOTOR2].integralError = 0;
	M[MOTOR2].feedforward = 0;
	M[MOTOR2].ff_multiplier = 0.5f;//0
	M[MOTOR2].intTarget = 0;
	M[MOTOR2].calcPwm = 0;
	M[MOTOR2].pwm = 0;
	M[MOTOR2].Ki = 0.1;
	M[MOTOR2].Kp = 1.0;
	M[MOTOR2].startOffset = 150;
	M[MOTOR2].overloadDelta = 300; // delta rpm of 300
	M[MOTOR2].overloadTime = 50; // for 5 sec at 0.1 sec interrupt
	M[MOTOR2].overloadCount = 0;
	M[MOTOR2].antiWindup = 0;
}



