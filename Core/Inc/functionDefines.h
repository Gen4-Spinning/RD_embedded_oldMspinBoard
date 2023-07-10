#ifndef __FUNCTIONDEFINES_H
#define __FUNCTIONDEFINES_H

void Homing_LEFT(int step,int length);


void MotorStructInit(void);
void UpdateMotorPID(int motorIndex);
void UpdateMotorPiecing(int motorIndex);
char RpmError(void);
char CheckAllMotors(void);
void UpdateMotorVoltage(int motorIndex,int pwmValue);
void AllTimerOn(void);
void StepMotorPID(int motorIndex);
void StepMotorPID_w_DCOffset(int motorIndex,int offset);
void ResetSecondaryMotor(void);
void UpdateMotorPID_w_DCOffset(int motorIndex,int dc_offset);

void calculateRpm(void);
void calculateRpm3(void);

char Pushbutton(void);

void LedOn(char index);
void LedOff(char index);
void LedToggle(char index); 


char InputSensor1(void);
char InputSensor2(void);
char InputSensor3(void);

void BedCalibSensorStatus(void);
char RightCalib(void);
char LeftCalib(void);
void CalculateDeltaPulse(void);
void BobbinBedAutoCalib(void);
void AutoCorrectBobbinBed(void);


void TowerLamp(char index);

void MotorDrive(char index);
void PauseMachine(void);
void calculateRpm(void);
void calculateRpm2(void);

char InputVoltageSense(void);
void RunMachine(void);

char InputSensor2(void);
char InputSensor1(void);
char MaxLayerComplete(void);
char LiftLimitCheck(void);

void ResetMotorVariables(uint8_t motorIndex);

#endif 
