#include "StateFns.h"
#include "Structs.h"
#include "CommonConstants.h"
#include "HMI_Constants.h"
#include "HMI_Fns.h"
#include "stm32f4xx_hal.h"
#include "functionDefines.h"
#include "machineControl.h"
#include "VFD.h"
#include "machineControl.h"

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart4;
extern TIM_HandleTypeDef htim7;
extern uint8_t BufferRec[];
extern char BufferTransmit[];
int changedSettings = 0;

void SettingsState(void)
{
	int sizeofPacket = 0;

	while(1){
		if (S.first_enter ==1){
			S.first_enter = 0;
		}
		
		if (S.state_change == TO_UPDATE_SETTINGS){
			S.state_change = 0; // so that u update settings only once.
			changedSettings =1;
			// change the settings in the CSP.Then send the ACK packet
			ms.inputYarnCountNe = msUpdate.inputYarnCountNe;
			ms.outputYarnDia = msUpdate.outputYarnDia;
			ms.spindleSpeed = msUpdate.spindleSpeed;
			ms.tpi = msUpdate.tpi;
			ms.packageHeight = msUpdate.packageHeight;
			ms.windingClosenessFactor= 	msUpdate.windingClosenessFactor;
			ms.windingOffsetCoils = msUpdate.windingOffsetCoils;
			ms.diaBuildFactor = msUpdate.diaBuildFactor;

			//then write the new values into the eeprom
			uint8_t dataWriteError = WriteMachineSettingsIntoEeprom(&ms);

			if (dataWriteError == 0){
				//update the rest of the machine params
				uint8_t spindleSpeedCode =  getSpindleSpeedCode(ms.spindleSpeed);
				VFD_setSpindleSpeed(&vfd, spindleSpeedCode); //set this based on settings
				SetupRDCalculations(&ms,&rd);

				// ACK PACKET
				UpdateBasePacket_Modes(CURRENT_MACHINE,HMI_BG_DATA,FROM_HMI_CHANGE_PROCESS_PARAMS,FROM_MC_ACK_UPDATED_SETTINGS,FROM_MC_ACK_PKT_LEN,NO_VAR);
				sizeofPacket = HMI_GetSettingsACKPacketString(BufferTransmit,hsb);
				HAL_UART_Transmit_IT(&huart1, (uint8_t *)&BufferTransmit, sizeofPacket);
				U.TXcomplete = 0;
				U.TXtransfer = 0;
			}

		}

		if (S.state_change == TO_IDLE){
				S.current_state =  IDLE_STATE;
				S.prev_state = UPDATESETTINGS;
				S.first_enter = 0; //Dont ALLOW THE RPI TO GET A MESG WHEN U GO BACK FROM SETTINGS
				changedSettings = 0;
				S.oneTimeflag = 0;
				HAL_TIM_Base_Stop_IT(&htim7);
				break;
			}
	} // closes while
							
}

