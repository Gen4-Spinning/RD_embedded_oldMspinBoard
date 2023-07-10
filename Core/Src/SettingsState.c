#include "StateFns.h"
#include "Structs.h"
#include "CommonConstants.h"
#include "HMI_Constants.h"
#include "HMI_Fns.h"
#include "stm32f4xx_hal.h"
#include "eeprom.h"
#include "functionDefines.h"

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart4;
extern TIM_HandleTypeDef htim7;
extern uint8_t BufferRec[];
extern char BufferTransmit[];
extern char RPIBufferTransmit[];
extern int RunLeft;
extern int RunRight;

int changedSettings = 0;
void SettingsState(void)
{
	int sizeofPacket = 0;
	while(1)
			{
				if (S.first_enter ==1)
						{	
							S.first_enter = 0;
						}	
								
		
							if (S.state_change == TO_UPDATE_SETTINGS)
							{
								S.state_change = 0; // so that u update settings only once.
								changedSettings =1;
								// change the settings in the CSP.Then send the ACK packet
								fsp.spindleSpeed = ufsp.spindleSpeed;
								fsp.draft = ufsp.draft;
								fsp.tpi = ufsp.tpi;
								fsp.yarnCount = ufsp.yarnCount;
								fsp.bindWindRatio = ufsp.bindWindRatio;
								fsp.chaseLength	 = 	ufsp.chaseLength;
								fsp.preferredPackageSize = ufsp.preferredPackageSize;
								fsp.rightSideOn = ufsp.rightSideOn;
								fsp.leftSideOn = ufsp.leftSideOn;
								
								RunLeft = fsp.leftSideOn;
								RunRight = fsp.rightSideOn;
								
								//then write the new values into the eeprom
								WriteRFSettingsIntoEeprom();
	
								// ACK PACKET
								UpdateBasePacket_Modes(CURRENT_MACHINE,HMI_BG_DATA,FROM_HMI_CHANGE_PROCESS_PARAMS,FROM_MC_ACK_UPDATED_SETTINGS,FROM_MC_ACK_PKT_LEN,NO_VAR);
								sizeofPacket = HMI_GetSettingsACKPacketString(BufferTransmit,hsb);
								HAL_UART_Transmit_IT(&huart1, (uint8_t *)&BufferTransmit, sizeofPacket);
								U.TXcomplete = 0; 
								U.TXtransfer = 0;
																												
							}
							
								
							if (S.state_change == TO_IDLE)
								{
									S.current_state =  IDLE_STATE;
									S.prev_state = UPDATESETTINGS;
									S.first_enter = 0; //Dont ALLOW THE RPI TO GET A MESG WHEN U GO BACK FROM SETTINGS
									changedSettings = 0;
									S.oneTimeflag = 0;
									HAL_TIM_Base_Stop_IT(&htim7);
									break;
								}
						
							}
							
}

