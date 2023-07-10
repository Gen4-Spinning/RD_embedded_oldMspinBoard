#include "StateFns.h"
#include "Structs.h"
#include "CommonConstants.h"
#include "HMI_Constants.h"
#include "HMI_Fns.h"
#include "stm32f4xx_hal.h"
#include "eeprom.h"
#include "functionDefines.h"
#include "logicDefines.h"
#include "main.h" 
#include "stdlib.h"
#include "../../Drivers/Encoder/encoder.h"

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart4;
extern TIM_HandleTypeDef htim7;
extern TIM_HandleTypeDef htim5;
extern TIM_HandleTypeDef htim13;
extern uint8_t BufferRec[];
extern char BufferTransmit[];

//logic varaibles for diagnostic test
extern char MOTORARRAY[6];
extern char MOTORARRAY_HMI[6];
extern int startFlag;
extern int pulseCount1;
extern int pulseCount2;
//global variables for testing defined here

int testMode = 0;
long diag_pwm = 0;
int idxMotor_diag = 0;	
int current_pwm_percent = 0;
char compareMotorID = 0;
int rpi_type_ofTest   = 0;
char finalStatus = 0;
int finalErrorDiag = 0;
int testOver = 0;

void DiagnosticsState(void){
	int sizeofPacket = 0;
	while(1){
		//start the timer
		if (HAL_TIM_Base_GetState(&htim7) ==  HAL_TIM_STATE_READY) { // if switched off
			HAL_TIM_Base_Start_IT(&htim7); // currently at 1 sec
		}
						
		// if youve got the signal that youve got the test vals from the HMI
		if (S.state_change == RUN_DIAG_TEST){
			S.oneSecTimer = 0;	//Reset the one sec timer to keep track of how long the test should run
			S.state_change = TO_DIAG;// so that we dont keep restarting the timer
			ResetMotorVariables(MOTOR1);
			ResetMotorVariables(MOTOR2);

			// set the HMIbasepacket values
			UpdateBasePacket_Modes(CURRENT_MACHINE,HMI_BG_DATA,HMI_DIAG_RESULTS,NO_VAR,19,4);
					
			// start the test mode after setting the variables to the correct vals
			for (int i=0;i< sizeof(MOTORARRAY_HMI);i++){
				compareMotorID = (char) D.motorID;
				if (MOTORARRAY_HMI[i] == compareMotorID)
				{
					idxMotor_diag = i;
					break;
				}
			}
						
			if (D.liftMode == 0){
				diag_pwm =  (MAX_PWM * D.targetSignal)/100; //set the open loop val
				M[idxMotor_diag].setRpm = D.targetRPM;		//set the target RPM
			}
						
			testMode = 1;
						
			//If the motor selected is  the lift motors, and the doff state is OLD DOFF STATE, dont let the lift motors run.
			//Instead just send back some message.Overwrite the testMode.
			if (S.doffState == CONTINUE_OLD_DOFF){
				if ((idxMotor_diag == 6) || ( idxMotor_diag == 7)){
					testMode = 0;
					testOver = 1;

					finalStatus = HMI_DIAG_TEST_LIFT_W_OLD_DOFF;
					sizeofPacket = HMI_Get_DiagOver_PacketString(BufferTransmit,hsb,finalStatus);
					//Because this runs only once, we have to force it to write this.
					HAL_UART_Transmit_IT(&huart1, (uint8_t *)&BufferTransmit, sizeofPacket);
					U.TXcomplete = 0;
					U.TXtransfer = 0;
				}
			}
					
		} // closes if first time u get run diag test
					
			
		if (testMode == 1){
			if (D.liftMode == 1){
				if (D.liftOver == 1){
						finalStatus = HMI_DIAG_TEST_SUCCESSFUL;
						testOver = 1;
						D.liftOver = 0;
						D.liftMode = 0;
						D.liftDirection = 0;
						D.liftMotor = 0;
						D.liftDistance =0 ;
					}
				}
			else{// if the normal kind of test	when the onesec timer goes more than the test time,stop the motor
				if (S.oneSecTimer > D.testTime){
					if (D.typeofTest == HMI_DIAG_OPEN_LOOP){
						finalStatus = HMI_DIAG_TEST_SUCCESSFUL;
					}
					else{
						finalErrorDiag = M[idxMotor_diag].presentRpm - D.targetRPM ;
						// send test over mesg,reset the motor variables to their defaults.
						if (abs(finalErrorDiag) <= 50) {
							finalStatus = HMI_DIAG_TEST_SUCCESSFUL;
						}
						else{
							finalStatus = HMI_DIAG_TEST_FAIL;
						}
					}
					testOver = 1;
				}
			}
									
			if (testOver == 1) {
				testMode = 0;
				testOver = 0;
				startFlag = 0;
				MotorDrive(DISABLE_D); //actually disable
				AllSignalVoltageLow(); // switch off all motors.

				ResetMotorVariables(MOTOR1);
				ResetMotorVariables(MOTOR2);
				ResetEncoderVariables(&encs);//reset encoder vals
				// send test over mesg, send a rpi msg, reset the motor variables to their defaults.
				sizeofPacket = HMI_Get_DiagOver_PacketString(BufferTransmit,hsb,finalStatus);
									
				//Because this runs only once, we have to force it to write this.
				HAL_UART_Transmit_IT(&huart1, (uint8_t *)&BufferTransmit, sizeofPacket);
				U.TXcomplete = 0;
				U.TXtransfer = 0;
																		
				//Reset the Diag Packets
				idxMotor_diag = 0;
				diag_pwm = 0;
			}
			else {  // if test not over
				// Send the diag live messages, but the app will not process it if its in Lift Mode.
				// send the HMI DIAG MSG
				current_pwm_percent = ((M[idxMotor_diag].pwm * 100)/MAX_PWM);
				sizeofPacket = UpdateDiagPacketString(BufferTransmit,hsb,hdp,D.typeofTest,D.motorID,current_pwm_percent,M[idxMotor_diag].presentRpm);
				if ((U.TXcomplete ==1) && (U.TXtransfer == 1)){ // TxTransfer signals when to send the data
					 	 HAL_UART_Transmit_IT(&huart1, (uint8_t *)&BufferTransmit, sizeofPacket);
						 U.TXcomplete = 0;
						 U.TXtransfer = 0;
					}
				}
		} //closes test mode == 1

			
			
		if (S.state_change == TO_IDLE){
			if (testMode == 1){ // stop the test
				testMode = 0;
				testOver = 0 ;

				// if lift mode, and lift still running, stop it
				if ((D.liftMode == 1) && ( D.liftOver== 0))
				{
					if (D.liftMotor == HMI_RD_LIFT_RIGHT){
						HAL_TIM_PWM_Stop_IT(&htim5,TIM_CHANNEL_4);
						pulseCount1 = 0;
					}
					if (D.liftMotor == HMI_RD_LIFT_LEFT){ // TO CHANGE
						HAL_TIM_PWM_Stop_IT(&htim13,TIM_CHANNEL_1);
						pulseCount2 = 0;
					}
				}
				D.liftOver = 0;
				D.liftMode = 0;

				startFlag = 0;
				MotorDrive(DISABLE_D);
				AllSignalVoltageLow(); // switch off all motors.
							
				ResetMotorVariables(MOTOR1); // put back original values
				ResetMotorVariables(MOTOR2);
				ResetEncoderVariables(&encs); // Reset encoder vals
				//Reset the Diag Packets
				idxMotor_diag = 0;
				diag_pwm = 0;
			}

			pulseCount1 = 0;
			pulseCount2 = 0;

			testOver = 0;

			S.current_state =  IDLE_STATE;
			S.prev_state = DIAGNOSTIC_STATE;
			S.first_enter = 1; // ALLOW THE RPI TO GET A MESG WHEN U GO BACK FROM DIAGNOSTICS
			S.oneTimeflag = 0;
			HAL_TIM_Base_Stop_IT(&htim7);
			U.TXcomplete = 1;

			D.liftOver = 0;
			D.liftMode = 0;
			D.liftDirection = 0;
			D.liftMotor = 0;
			D.liftDistance =0 ;

			//Reset incase youve found an error!
			E.RpmErrorFlag = 0;
			break;
		}

		if (E.RpmErrorFlag == 1) {
			//switch of the test
			testMode = 0;
			testOver = 0 ;

			/*cant detect error when running Lift mode*/

			MotorDrive(DISABLE_D);
			AllSignalVoltageLow(); // switch off all motors.
			ResetMotorVariables(MOTOR1);
			ResetMotorVariables(MOTOR2);

			finalStatus = HMI_DIAG_TEST_MOTORERROR;
			sizeofPacket = HMI_Get_DiagOver_PacketString(BufferTransmit,hsb,finalStatus);
			if ((U.TXcomplete ==1) && (U.TXtransfer == 1)){
				HAL_UART_Transmit_IT(&huart1, (uint8_t *)&BufferTransmit, sizeofPacket);
				U.TXcomplete = 0;
				U.TXtransfer = 0;
			}
						
			//Reset the Diag Packets
			idxMotor_diag = 0;
			diag_pwm = 0;
						
		}
						
	}//closes while
							
}


