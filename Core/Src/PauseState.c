#include "StateFns.h"
#include "Structs.h"
#include "CommonConstants.h"
#include "HMI_Constants.h"
#include "HMI_Fns.h"
#include "stm32f4xx_hal.h"
#include "logicDefines.h"
#include "functionDefines.h"
#include "eeprom.h"
#include "VFD.h"
#include "main.h"
#include "../../Drivers/Encoder/encoder.h"

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart4;
extern TIM_HandleTypeDef htim7;
extern uint8_t BufferRec[];
extern char BufferTransmit[];

extern int allMotorsOn;
extern int stopSecondaryMotor;
extern int startFlag;
extern int sliverCut;
extern int chaseCount;
extern int pulseCount1;
extern int pulseCount2;
extern int directionChange;
extern int directionChange1;

void PauseState(void)
{ 
	char sizeofPacket = 0;
	char keypress = 0;
	char update_HMI_packet= 0;
	while(1){

		if (S.first_enter ==1){
			S.first_enter = 0;
		}
				
		// and start the timer that signals when to send the data.
		if (HAL_TIM_Base_GetState(&htim7) ==  HAL_TIM_STATE_READY){
			HAL_TIM_Base_Start_IT(&htim7); // currently at 1 sec
		}
						
		if ((update_HMI_packet== 0) || (S.first_enter == 1)){
			// keep it here because this will run once and we want this buffer packet to get created only once.
			UpdateBasePacket_Modes(CURRENT_MACHINE,HMI_SCREEN_DATA,HMI_STOP_SCREEN,HMI_STOP_PAUSE,HMI_STOP_PACKET_LENGTH,3);
			UpdateStopPacket(HMI_STOP_REASON_CODE,NO_VAR,NO_VAR);
			update_HMI_packet =1;
		}
						
		//send Pause packet
		if ((U.TXcomplete ==1) && (U.TXtransfer == 1)){
		//keep the error val 0.01, so that the float to hex conversion gives 8 bytes.
		 sizeofPacket = UpdateStopPacketString(BufferTransmit,hsb,hsp,S.errStopReasonHMI,S.errmotorFault,S.errVal);
		 HAL_UART_Transmit_IT(&huart1, (uint8_t *)&BufferTransmit, sizeofPacket);
		 U.TXcomplete = 0;
		 U.TXtransfer = 0;
		}

		TowerLamp(GREEN_OFF); // GREEN OFF
		TowerLamp(RED_OFF); //RED OFF
		TowerLamp(AMBER_ON); // AMBER ON

		/******************MOTOR LOGIC*******************/

		// NO LOGIC MOTOR WILL HALT IN INTERRUPT LOOP ONLY
			
		/*************************************************/
		if (allMotorsOn ==0){
			MotorDrive(DISABLE_D);
		}

		//TODO : WHATS THIS?
		/*
		if ((S.keyState == ENABLE) && ( S.currentStateSaved == 0)){
			EepromWriteInt(RF_CURRENT_CHASE_COUNT,chaseCount);
			EepromWriteInt(RF_CURRENT_PULSECOUNT1,pulseCount1);
			EepromWriteInt(RF_CURRENT_PULSECOUNT2,pulseCount2);
			EepromWriteInt(RF_CURRENT_DIRECTION_LEFT,directionChange1);
			EepromWriteInt(RF_CURRENT_DIRECTION_RIGHT,directionChange);
			EepromWriteInt(RF_SAVED_SETTINGS,1);
			S.currentStateSaved = 1;
		}*/
						
											
		//Check for button pres to go back to run state
		keypress = Pushbutton();
		if ((keypress==1) && (S.keyState == ENABLE_KEYPRESS)){

			VFD_startInductionMotor(&vfd);

			S.state_change = TO_RUN;
			S.current_state =  RUN_STATE;
			S.prev_state = PAUSE_STATE;
			S.first_enter = 1;  // send a rpimessage hen u go back to run
			S.oneTimeflag = 0;
			S.changeRunMode = 1;
			S.errStopReason = NO_VAR;
			S.errmotorFault = NO_VAR;
			S.errVal = NO_VAR;
			HAL_TIM_Base_Stop_IT(&htim7);
				
			TimerLow(T1_CH4);

			ResetSecondaryMotor();
			ResetEncoderVariables(&encs);
			MotorDrive(ENABLE_D);

			stopSecondaryMotor = 0;
			startFlag = 1;
			allMotorsOn = 1;

			LedOff(LED1);

			HAL_Delay(200);

			S.currentStateSaved = 0;
			break;
		}

	}
}

