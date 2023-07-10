
//Constants common to Logic, and the two Communication channels

//GENERAL
#define OFF 0
#define ON 1
#define UP 1
#define DOWN 2

//current machine SETTINGs
#define CURRENT_MACHINE RINGFRAME
#define CURRENT_MACHINE_ID 1

//MACHINES

#define CARDING 1
#define DRAWFRAME 2
#define FLYER 3
#define RINGFRAME 4


//STATES
#define OFF_STATE 0
#define IDLE_STATE 1
#define DIAGNOSTIC_STATE 2
#define RUN_STATE 3
#define PAUSE_STATE 4
#define HALT_STATE 5
#define HOMING_STATE 6
#define UPDATESETTINGS 7 // NOT SURE IF WE NEED THIS

//change states
#define TO_RUN 0
#define TO_DIAG 1
#define TO_SETTINGS 2
#define TO_PAUSE 3
#define TO_HALT 4
#define TO_UPDATE_SETTINGS 5
#define TO_IDLE 6
#define TO_HOMING 7
#define TO_NEW_DOFF 8

#define NEW_DOFF 1
#define CONTINUE_OLD_DOFF 2

//MOTORS
#define RD_FRONTROLLER_RIGHT 0x70
#define RD_FRONTROLLER_LEFT 0x72
#define RD_LIFT_FRONT 0x74
#define RD_LIFT_BACK 0x75
#define RD_SPINDLE 0x76

//DIAG CODES. HMI AND RPI have different test type codes. TO FIX in next round
#define DIAG_ATTR_MOTORID 0x01
#define DIAG_ATTR_SIGNAL_IP_PERCENT 0x02
#define DIAG_ATTR_TARGET_RPM 0x03
#define DIAG_ATTR_TEST_TIME 0x04
#define DIAG_ATTR_TEST_RESULT 0x05
//forLIFT
#define DIAG_ATTR_LIFT_DIRECTION 0x06
#define DIAG_ATTR_LIFT_DIST 0x07
//direction attributes
#define LIFT_UP 0x01
#define LIFT_DOWN 0x02


// ERROR CODES
#define ERR_RPM_ERROR 0x02
#define ERR_MOTOR_VOLTAGE_ERROR 0x03
#define ERR_DRIVER_VOLTAGE_ERROR 0x04
#define ERR_BOBBIN_BED_LIFT 0x05
#define ERR_LAYERS_COMPLETE 0x06
#define ERR_SLIVER_CUT_ERROR 0x07
#define ERR_USER_PAUSE 0x08

//DATATYPES
#define FLOAT 4
#define INT 2
#define CHAR 1

//MISC
#define NO_VAR 0x00
#define NO_FLOAT_VAR (float)0.01
#define	FRONTROLLER_OD_MM 30

