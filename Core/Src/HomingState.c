#include "main.h"
#include "StateFns.h"
#include "Structs.h"
#include "CommonConstants.h"
#include "HMI_Constants.h"
#include "HMI_Fns.h"
#include "stm32f4xx_hal.h"
#include "functionDefines.h"
#include "logicDefines.h"

#include "../../Drivers/Encoder/encoder.h"

extern UART_HandleTypeDef huart4;
extern UART_HandleTypeDef huart1;
extern TIM_HandleTypeDef htim7;
extern TIM_HandleTypeDef htim5;
extern TIM_HandleTypeDef htim13;
extern uint8_t BufferRec[];
extern char BufferTransmit[];

extern int homeFlag ;
extern int hommingFlag;
extern int machineRun;
extern int pulseCount2;
extern int pulseCount1;

char homing_Calib_State = DOWN;
char homing_Normal_StateFront = OFF;
char homing_Normal_StateBack = OFF;
char btm_bed_reachedFront = 0;
char btm_bed_reachedBack = 0;
char leftSensor = 0;
char rightSensor = 0;
int leftMotorOn = 0;
int rightMotorOn = 0;
extern int doffPercent;

extern int RunLeft;
extern int RunRight;

void HomingState(void)
{ 
	int sizeofPacket = 0;
	int amberisON = 1;
	while(1){
		if (S.first_enter ==1){ // is only used for sending RPI info
			// create the run packet for the HMI
			UpdateBasePacket_Modes(CURRENT_MACHINE,HMI_SCREEN_DATA,HMI_RUN_SCREEN,HMI_RUN_HOMING,HMI_RUN_PACKET_LENGTH_RF,2);
			UpdateRunPacket_RF(HMI_RF_SPINDLESPEED,fsp.spindleSpeed,HMI_RF_DOFFPERCENT,doffPercent);
													
			homing_Calib_State = DOWN;
			homing_Normal_StateFront = DOWN;  //changed from off to down
			homing_Normal_StateBack = DOWN;

			homeFlag = 1;
			MotorDrive(DISABLE_D);   // All drive Enable pin high
			leftMotorOn = 1;
			rightMotorOn = 1;
						
			S.first_enter = 0;
		}
						
					// and start the timer that signals when to send the data.
		if (HAL_TIM_Base_GetState(&htim7) ==  HAL_TIM_STATE_READY){ // if switched off
				HAL_TIM_Base_Start_IT(&htim7); // currently at 1 sec
			}

		// every sec, send a Homing screen(if you send it once, if it fails your gone)
		if( (U.TXtransfer == 1)	&& (U.TXcomplete == 1)){
				sizeofPacket = UpdateRunPacketString_RF(BufferTransmit,hsb,hrp,fsp.spindleSpeed,doffPercent);
				HAL_UART_Transmit_IT(&huart1, (uint8_t *)&BufferTransmit, sizeofPacket);
				U.TXcomplete = 0;
				U.TXtransfer = 0;
				amberisON *= -1;
				if (amberisON > 0){
					TowerLamp(AMBER_ON);
				}else{
					TowerLamp(AMBER_OFF);
				}
			}
							
															
		//Logic for FRONT SIDE (LEFT)
		if (RunRight == 1){
			if (homing_Normal_StateFront == DOWN){
				HAL_GPIO_WritePin(GPIOA,STP1_DIR_Pin,GPIO_PIN_RESET); //direction DOWN
				homeFlag = 1;
				btm_bed_reachedFront = InputSensor1();
				pulseCount1 = 0; // keepmaking this zero so that they dont trigger a dir change

				if (btm_bed_reachedFront == 1){
					//Reverse Direction
					HAL_GPIO_WritePin(GPIOA,STP1_DIR_Pin,GPIO_PIN_SET);
					homing_Normal_StateFront = UP;
					}
				}

				if (homing_Normal_StateFront == UP){
					if (pulseCount1 >= 750){ // HARDCODED DISTANCE
						homing_Normal_StateFront = OFF;
						HAL_TIM_PWM_Stop(&htim5,TIM_CHANNEL_4);
						pulseCount1 = 0;
					}
				}
			}else{
				homing_Normal_StateFront = OFF;
			}
							
		//Logic for BACK SIDE
		if (RunLeft == 1){
			if (homing_Normal_StateBack == DOWN){
				HAL_GPIO_WritePin(STP2_DIR_NEW_GPIO_Port,STP2_DIR_NEW_Pin,GPIO_PIN_RESET); //direction DOWN
				homeFlag = 1;
				btm_bed_reachedBack = InputSensor2();
				pulseCount2 = 0; // keepmaking this zero so that they dont trigger a dir change

				if (btm_bed_reachedBack == 1){
					//Reverse Direction
					HAL_GPIO_WritePin(STP2_DIR_NEW_GPIO_Port,STP2_DIR_NEW_Pin,GPIO_PIN_SET);
					homing_Normal_StateBack = UP;
					}
				}

				if (homing_Normal_StateBack == UP)
				{
					if (pulseCount2 >= 750){ // HARDCODED DISTANCE WRT SENSOR LOC
						homing_Normal_StateBack = OFF;
						HAL_TIM_PWM_Stop(&htim13,TIM_CHANNEL_1);
						pulseCount2 = 0;
					}
				}
			}else{
				homing_Normal_StateBack = OFF;
			}

			if ((homing_Normal_StateBack == OFF) && (homing_Normal_StateFront == OFF)){
				homeFlag = 0;
				S.state_change = TO_IDLE;
				machineRun = 1;
				hommingFlag = 0;
				HAL_Delay(500);
			}

						
			if (S.state_change == TO_IDLE){
				S.current_state =  IDLE_STATE;
				S.prev_state = HOMING_STATE;
				S.first_enter = 1; //  ALLOW THE RPI TO GET A MESG WHEN U GO BACK FROM HOMING
				S.oneTimeflag = 0;
				HAL_TIM_Base_Stop_IT(&htim7);
				U.TXcomplete = 1;
				break;
			}

		} // closes while
}
		

