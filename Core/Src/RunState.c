#include "StateFns.h"
#include "Structs.h"
#include "CommonConstants.h"
#include "HMI_Constants.h"
#include "HMI_Fns.h"
#include "stm32f4xx_hal.h"
#include "functionDefines.h"
#include "logicDefines.h"
#include "VFD.h"
#include "machineControl.h"
#include "../../Drivers/Encoder/encoder.h"

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart4;
extern TIM_HandleTypeDef htim7;
extern uint8_t BufferRec[];
extern char BufferTransmit[];
extern char MOTORARRAY[];
extern char MOTORARRAY_HMI[];

//RUN LOGIC variables
extern int allMotorsOn;
extern int stopSecondaryMotor;
extern int startFlag;

extern int currentLayer;
extern int homeFlag;
extern int pulseCount1;
extern int pulseCount2;

uint8_t leftFinished = 0;
uint8_t rightFinished = 0;

void RunState(void){
	char keyPress = 0;
	char sizeofPacket = 0;
	while(1){
		/******************COMMUNICATION**************/
		if (S.first_enter ==1){
			// prepare the Run packet Structure for the HMI
			UpdateBasePacket_Modes(CURRENT_MACHINE,HMI_SCREEN_DATA,HMI_RUN_SCREEN,HMI_RUN_NORMAL,HMI_RUN_PACKET_LENGTH_RF,3);
			UpdateRunPacket_RF(HMI_RF_SPINDLESPEED,vfd.presentRPM,HMI_RD_LEFT_DOFFPERCENT,rd.doffPercentLeft,HMI_RD_RIGHT_DOFFPERCENT,rd.doffPercentRight);

			TowerLamp(GREEN_ON); // GREEN ON
			TowerLamp(AMBER_OFF); // AMBER OFF
			TowerLamp(RED_OFF); //RED OFF

			S.changeRunMode = 0;
			S.first_enter = 0;
		}

		// start the timer that signals when to send the data.
		if (HAL_TIM_Base_GetState(&htim7) ==  HAL_TIM_STATE_READY){
			//Make the timer a half sec timer here
			__HAL_TIM_SetAutoreload(&htim7,4200);
			HAL_TIM_Base_Start_IT(&htim7); // currently at 1 sec
		}

		//send Run packet
		if ((U.TXcomplete ==1) && (U.TXtransfer == 1)){
			sizeofPacket = UpdateRunPacketString_RF(BufferTransmit,hsb,hrp,vfd.presentRPM,rd.doffPercentLeft,rd.doffPercentRight);
			HAL_UART_Transmit_IT(&huart1, (uint8_t *)&BufferTransmit, sizeofPacket);
			U.TXcomplete = 0;
			U.TXtransfer = 0;
		}
				

		/***********MAIN MOTOR LOGIC************************/
		if (S.firstSwitchon == 1){

			/* turn on the VFD */
			VFD_startInductionMotor(&vfd);

			ResetSecondaryMotor();
			ResetEncoderVariables(&encs);
			MotorDrive(ENABLE_D);

			// logic variables
			allMotorsOn = 1;
			stopSecondaryMotor= 0; //
			startFlag = 1;
					
			homeFlag = 0;
	
			S.firstSwitchon = 0;
			HAL_Delay(2000);
		}

					
		//Check for User KeyPress to take machine into Pause State
		keyPress = Pushbutton();
		if (keyPress){
			/* pause induction motor*/
			VFD_stopInductionMotor(&vfd);

			S.state_change = TO_PAUSE;
			S.current_state =  PAUSE_STATE;
			S.prev_state = RUN_STATE;
			S.first_enter = 1;
			S.oneTimeflag = 0;
			S.errStopReason = ERR_USER_PAUSE;
			S.errStopReasonHMI = ERR_USER_PAUSE;
			S.errmotorFault = NO_VAR;
			S.errVal = NO_FLOAT_VAR;
			S.keyState = DISABLE_KEYPRESS;
			HAL_TIM_Base_Stop_IT(&htim7);
			__HAL_TIM_SetAutoreload(&htim7,8400); //back to one sec timer

			stopSecondaryMotor = 1;
			startFlag = 0; // will make the motor RAMP DOWN

			HAL_Delay(200);
			break;
		}
				
		/* if all layers over */
		leftFinished = LeftDoffOver(&rd);
		rightFinished = RightDoffOver(&rd);
		//TODO send both doff percents

		if ((leftFinished == 1) || (rightFinished == 1)){
		   // just pause both sides and then halt.
			if (allMotorsOn == 1){
				startFlag = 0;
				stopSecondaryMotor = 1;
			}
			else{//go to halt
				E.RpmErrorFlag = 1;
				S.errStopReason = ERR_LAYERS_COMPLETE;
				S.errStopReasonHMI = ERR_LAYERS_COMPLETE;
				S.errmotorFault = NO_VAR;
				S.errVal = NO_VAR;
			}
		}
						
		//Check for RPM ERROR, to go into halt State
		if (E.RpmErrorFlag == 1){
			S.state_change = TO_HALT;
			S.current_state = HALT_STATE;
			S.prev_state = RUN_STATE;

			S.first_enter = 1;
			S.oneTimeflag = 0;
			HAL_TIM_Base_Stop_IT(&htim7);
			__HAL_TIM_SetAutoreload(&htim7,8400); //back to one sec timer
			HAL_Delay(200);
			break;
		}

	} // closes while
}



