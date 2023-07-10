#ifndef __STRUCTS_H
#define __STRUCTS_H

struct Uart
{
	volatile char TXcomplete ; //Flag that says whether a uart transmission is complete. is set to 1 inside the TX finished interrupt
										// so make it 0 after every transmission call
	volatile char TXtransfer; // Flag used in run mode and diagnosic mode where there is continuous data to link a timer with the logic code.
	volatile char RPI_TXcomplete;
};

struct State 
{	
	char current_state; //current state
	char prev_state;    // prev state
	char paired;				// paired with HMI bool
	char state_change;	// bool to actually force a state change
	char first_enter;		// flag to tell when u enter a state for the first time
	char oneTimeflag;   // bool flag which can be used for any thing  
	char errStopReason;  //stop reason,which is usually the faulty motor 
	char errStopReasonHMI ;// stop reason with hmi code
	char errmotorFault; // reason why the motor stopped
	float errVal;				// err value associated with the reason the mtoor stopped
	char firstSwitchon; // state that says when we first switch on the machine
	char runMode;				// the current run state, normal, ramp or piecing.(FOR HMI)
	char changeRunMode; // flag when u change run state (FOR HMI)
	char RPI_idleSettingsTransferOnce  ; // with the RPI, need to send settings once when in idle the first time.
	int oneSecTimer;
	float displayRTF;
	float runningRTF;
	int keyState;
	int currentStateSaved;
	int doffState;
	int savedStateExists;
};

struct FlyerSettings
{
	unsigned int spindleSpeed;
	unsigned int draft;
	unsigned int tpi;
	unsigned int yarnCount;
	float bindWindRatio;
	float chaseLength;
	unsigned preferredPackageSize;
	unsigned int rightSideOn;
	unsigned int leftSideOn;
};

struct UpdateFlyerSettings
{
	unsigned int spindleSpeed;
	unsigned int draft;
	unsigned int tpi;
	unsigned int yarnCount;
	float bindWindRatio;
	float chaseLength;
	unsigned preferredPackageSize;
	unsigned int rightSideOn;
	unsigned int leftSideOn;

};


struct Diagnostics
{
    int typeofTest;
    int motorID;
    int targetRPM;
    long targetSignal;
    int testTime;
		int liftDirection;
		int liftDistance;
		int liftMode;
		int liftOver;
		int liftMotor;
};

/*
struct Motor 
{	
	unsigned int setRpm;	
	unsigned int presentRpm;
	float rampRpm;
	unsigned int piecingRpm;
	signed int intTarget;
	signed int error;
	signed int integralError;
	signed int derivativeError;
	signed int preError;
	unsigned int pwm;
	float Kp;
	float Ki;
	float Kd;
};*/

struct Motor{
	unsigned int setRpm;
	unsigned int presentRpm;
	float rampRpm;
	unsigned int piecingRpm;
	float interimTarget;
	signed int intTarget;
	signed int error;
	signed int integralError;
	unsigned int feedforward;
	float ff_multiplier;
	signed int calcPwm;
	signed int pwm;
	float Kp;
	float Ki;
	int startOffsetOrig; //startOffset gets overwritten in the code. so to keep a non changing version
	int startOffset;
	int overloadDelta;
	int overloadTime;
	int overloadCount;
	int antiWindup;
};

// TO keep structs for logic and for communication different
struct Error 
{
	unsigned int RpmErrorFlag;
};


extern struct Motor M[6];
extern struct State S;
extern struct FlyerSettings fsp;
extern struct UpdateFlyerSettings ufsp;
extern struct Uart U;
extern struct Error E;
extern struct Diagnostics D;
#endif
