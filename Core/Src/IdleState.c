#include "StateFns.h"
#include "Structs.h"
#include "CommonConstants.h"
#include "HMI_Constants.h"
#include "HMI_Fns.h"
#include "stm32f4xx_hal.h"
#include "functionDefines.h"
#include "logicDefines.h"
#include "eeprom.h"
#include "../../Drivers/Encoder/encoder.h"

extern UART_HandleTypeDef huart4;
extern UART_HandleTypeDef huart1;
extern TIM_HandleTypeDef htim7;
extern uint8_t BufferRec[];
extern char BufferTransmit[];

char doNothing = 0;
extern int directionChange;
extern int directionChange1;
extern int chaseCount;

int hommingFlag = 1 ; //flag to tell idle to go to homing onfirst key press
int machineRun = 0; //flag to tell idle to go to run state
HAL_StatusTypeDef returnVal;
int count = 0;
extern int RunLeft;
extern int RunRight;
int resetDirections = 0;
extern int pulseCount1;
extern int pulseCount2;
extern int currentLayer;
char keyPress_idle = 0;

void IdleState(void)
{ 
	int sizeofPacket = 0;
	
	while(1){
		// and start the timer that signals when to send the data.
		if (HAL_TIM_Base_GetState(&htim7) ==  HAL_TIM_STATE_READY){ // if switched off
			HAL_TIM_Base_Start_IT(&htim7); // currently at 1 sec
		}

		TowerLamp(AMBER_ON);
		TowerLamp(RED_OFF);
		TowerLamp(GREEN_OFF);

		// every sec, send a Idle screen(if you send it once, if it fails your gone)
		if((U.TXtransfer == 1)	&& (U.TXcomplete == 1)){
			UpdateBasePacket_Modes(CURRENT_MACHINE,HMI_SCREEN_DATA,HMI_IDLE_SCREEN,NO_VAR,NO_VAR,NO_VAR);
			sizeofPacket = HMI_GetIdlePacketString(BufferTransmit,hsb);
			HAL_UART_Transmit_IT(&huart1, (uint8_t *)&BufferTransmit, sizeofPacket);

			//every three sec send the new doff msg incase we miss out the first time
			if ((S.oneSecTimer%3==0) && (S.savedStateExists !=1)){
				UpdateBasePacket_Modes(CURRENT_MACHINE,HMI_BG_DATA,HMI_SETTINGS_INFO,RESET_DOFF,FROM_MC_ACK_PKT_LEN,NO_VAR);
				sizeofPacket = HMI_GetSettingsACKPacketString(BufferTransmit,hsb);
				HAL_UART_Transmit_IT(&huart1, (uint8_t *)&BufferTransmit, sizeofPacket);
			}

			U.TXcomplete = 0;
			U.TXtransfer = 0;
		}
						
		RunRight = 1;
		RunLeft = 1;

		/*********MOTOR LOGIC************/
		/*If there doesnt exist any currently saved state with which you can continue,
			force the RD to go into homing state */
		if (S.savedStateExists	== 0){
			chaseCount = 0;
			pulseCount1 = 0;
			pulseCount2 = 0 ;

			S.doffState = NEW_DOFF;

			UpdateBasePacket_Modes(CURRENT_MACHINE,HMI_BG_DATA,HMI_SETTINGS_INFO,RESET_DOFF,FROM_MC_ACK_PKT_LEN,NO_VAR);
			sizeofPacket = HMI_GetSettingsACKPacketString(BufferTransmit,hsb);
			HAL_UART_Transmit_IT(&huart1, (uint8_t *)&BufferTransmit, sizeofPacket);
			U.TXcomplete = 0;
			U.TXtransfer = 0;

		  S.savedStateExists = 255;
		}
								
		keyPress_idle = Pushbutton();

		//Start Homing
		if((keyPress_idle == 1)&& (S.doffState == NEW_DOFF)){
			S.state_change = TO_HOMING;
			S.current_state =  HOMING_STATE;
			S.prev_state = IDLE_STATE;
			S.first_enter = 1;
			S.oneTimeflag = 0;
			HAL_TIM_Base_Stop_IT(&htim7);
			S.doffState  = CONTINUE_OLD_DOFF;
			resetDirections = 1;
			break;
		}


		if((keyPress_idle == 1)&& (S.doffState == CONTINUE_OLD_DOFF)){
			//for when your coming from homing
			if (resetDirections == 1){
				//Reset the direction your going to use when you come out
				directionChange = -1;
			    directionChange1 = -1;
				pulseCount1 = 0;
				pulseCount2 = 0;
				currentLayer = 0;
			}
			// write Zero into the savedState
			EepromWriteInt(RF_SAVED_SETTINGS,0);
						
			S.state_change = TO_RUN;
			S.current_state =  RUN_STATE;
			S.prev_state = IDLE_STATE;
			S.first_enter = 1;
			S.oneTimeflag = 0;
			S.changeRunMode = 1; // only when going to RUN, need a flag to set the correct state
			HAL_TIM_Base_Stop_IT(&htim7);
			HAL_Delay(500);
			break;
		}

		/*******************************************/
					
		if (S.state_change == TO_DIAG){
				S.current_state =  DIAGNOSTIC_STATE;
				S.prev_state = IDLE_STATE;
				S.first_enter = 1;
				S.oneTimeflag = 0;
				HAL_TIM_Base_Stop_IT(&htim7);
				break;
			}
			
		if (S.state_change == TO_SETTINGS){
			S.current_state =  UPDATESETTINGS;
			S.prev_state = IDLE_STATE;
			S.first_enter = 1;
			S.oneTimeflag = 0;
			HAL_TIM_Base_Stop_IT(&htim7);
			break;
		}
					
	} //closes while
}

