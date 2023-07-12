#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include "HMI_Fns.h"
#include "HMI_Constants.h"
#include "CommonConstants.h"
#include "stm32f4xx_hal.h"
#include "stdio.h"
#include "machineControl.h"

extern int pulseCount1;
extern int pulseCount2;

void Create_HMI_BasePacket(void)
{
    hsb.start = HMI_START_END_CHAR;
    hsb.sender = MACHINE_TO_HMI;
    hsb.packetLength = 12; // gets updated
    hsb.machineID = CURRENT_MACHINE_ID;
    hsb.machineType = CURRENT_MACHINE; // gets updated
    hsb.msgType = HMI_SCREEN_DATA; // gets updated
    hsb.nextscreen =  HMI_RUN_SCREEN; // gets updated
    hsb.screen_substate = HMI_RUN_RAMPUP; // gets updated
    hsb.attributeCount = 3; // gets updated
    hsb.endChar = HMI_START_END_CHAR;
}

void Create_HMI_Run_Packet(void)
{
    hrp.tlv1Code = HMI_RF_PRODUCTION_RUN;
    hrp.tlv1length = 2;
    hrp.tlv1_val = 0;
}


void Create_HMI_StopPacket(void)
{
    hsp.tlv1Code = HMI_STOP_REASON_CODE;
    hsp.tlv1length = 2;
    hsp.tlv1_val = HMI_RD_BACKROLLER_BACK;
    hsp.tlv2Code = HMI_MOTOR_FAULT_CODE;
    hsp.tlv2length = 2;
    hsp.tlv2_val = ERR_RPM_ERROR;
    hsp.tlv3Code = HMI_ERROR_VAL_CODE;
    hsp.tlv3length =4;
    hsp.tlv3_val = 40;
};

void Create_HMI_DiagPacket(void)
{
    hdp.tlv1Code = HMI_DIAG_TEST_CODE;
    hdp.tlv1length = 2;
    hdp.tlv1_val = 0;
    hdp.tlv2Code = DIAG_ATTR_MOTORID;
    hdp.tlv2length = 2;
    hdp.tlv2_val = 0;
    hdp.tlv3Code = DIAG_ATTR_SIGNAL_IP_PERCENT;
    hdp.tlv3length = 2;
    hdp.tlv3_val = 0;
    hdp.tlv4Code = DIAG_ATTR_TARGET_RPM;
    hdp.tlv4length = 2;
    hdp.tlv4_val = 0;
}



void UpdateBasePacket_Modes(char machineType,char msgType,char state,char runsubMode,char packetLength,char attributeCount)
{
    hsb.msgType = msgType;
    hsb.machineType = machineType;
    hsb.nextscreen = state;
    hsb.screen_substate = runsubMode;
    hsb.packetLength = packetLength;
    hsb.attributeCount = attributeCount;
}

void UpdateRunPacket_RF(char tlv1Code,int tlv1Val,char tlv2Code,int tlv2Val,char tlv3Code,int tlv3Val)
{
    hrp.tlv1Code = tlv1Code;
    hrp.tlv1length = 2;
    hrp.tlv1_val = tlv1Val;
    hrp.tlv2Code = tlv2Code;
    hrp.tlv2length = 2;
    hrp.tlv2_val = tlv2Val;  
    hrp.tlv3Code = tlv3Code;
    hrp.tlv3length = 2;
    hrp.tlv3_val = tlv3Val;
}

void UpdateStopPacket(char tlv1Code,char tlv2Code,char tlv3Code)
{
    hsp.tlv1Code = tlv1Code;
    hsp.tlv2Code = tlv2Code;
    hsp.tlv3Code = tlv3Code;
	hsp.tlv3length = 2;
		
}

int UpdateRunPacketString_RF(char *buffer,struct HMI_InfoBase hsb,struct HMI_RunPacket hrp,int spindleSpeed,int leftdoffPercent,int rightdoffPercent)
{    int sizeofPacket = 0;

     sprintf(buffer,"%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%04X%02X%02X%04X%02X%02X%04X%02X\r",hsb.start,hsb.sender,hsb.packetLength,
		 hsb.machineID,hsb.machineType,hsb.msgType,hsb.nextscreen,hsb.screen_substate,hsb.attributeCount,hrp.tlv1Code,
		 hrp.tlv1length,spindleSpeed,hrp.tlv2Code,hrp.tlv2length,leftdoffPercent,hrp.tlv3Code,hrp.tlv3length,rightdoffPercent,hsb.endChar);
		 sizeofPacket = 45;
  
    return sizeofPacket;
}

int UpdateStopPacketString(char *buffer,struct HMI_InfoBase hsb,struct HMI_StopPacket hsp,int stop_reason,int motor_fault,float error_val)
{   int sizeofPacket = 0;
	
		int error_val1 = 0; // error val in rpi is a float, but here it is an int.
		sprintf(buffer,"%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%04X%02X%02X%04X%02X%02X%04X%02X\r",hsb.start,hsb.sender,hsb.packetLength,
		hsb.machineID,hsb.machineType,hsb.msgType,hsb.nextscreen,hsb.screen_substate,hsb.attributeCount,hsp.tlv1Code,
		hsp.tlv1length,stop_reason,hsp.tlv2Code,hsp.tlv2length,motor_fault,hsp.tlv3Code,hsp.tlv3length,
		error_val1,hsb.endChar);
		sizeofPacket = 45;
		
    return sizeofPacket;
}

int UpdatePausePacketString(char *buffer,struct HMI_InfoBase hsb,struct HMI_StopPacket hsp,int stop_reason,int motor_fault,float error_val)
{   int sizeofPacket = 0;
	
		sprintf(buffer,"%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%04X%02X%02X%04X%02X%02X%08X%02X\r",hsb.start,hsb.sender,hsb.packetLength,
		hsb.machineID,hsb.machineType,hsb.msgType,hsb.nextscreen,hsb.screen_substate,hsb.attributeCount,hsp.tlv1Code,
		hsp.tlv1length,stop_reason,hsp.tlv2Code,hsp.tlv2length,motor_fault,hsp.tlv3Code,hsp.tlv3length,
		*(unsigned int*)&error_val,hsb.endChar);
		sizeofPacket = 49;
		
    return sizeofPacket;
}


int HMI_GetRDSettingsAllPacketString(char *buffer,struct HMI_InfoBase hsb,machineSettingsTypeDef ms)
{ int sizeofPacket = 0;
  sprintf(buffer,"%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02d%04X%X%04X%04X%04X%X%04X%X%02X\r",hsb.start,hsb.sender,
         hsb.packetLength,hsb.machineID,hsb.machineType,hsb.msgType,hsb.nextscreen,hsb.screen_substate,hsb.attributeCount,
         HMI_RF_ALL_SETTING_CODE,HMI_RF_ALL_SETTINGS_PKT_LEN,ms.inputYarnCountNe,*(unsigned int*)&ms.outputYarnDia,ms.spindleSpeed,
				ms.tpi,ms.packageHeight,*(unsigned int*)&ms.diaBuildFactor,ms.windingClosenessFactor,
				*(unsigned int*)&ms.windingOffsetCoils,hsb.endChar);
  sizeofPacket = 69;
  return sizeofPacket;
}

int HMI_GetIdlePacketString(char *buffer,struct HMI_InfoBase hsb)
{	int sizeofPacket = 0;
  sprintf(buffer,"%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X\r",hsb.start,hsb.sender,
         hsb.packetLength,hsb.machineID,hsb.machineType,hsb.msgType,hsb.nextscreen,hsb.screen_substate,hsb.attributeCount,
         hsb.endChar);
  sizeofPacket = 21;
  return sizeofPacket;
	
}

int HMI_GetSettingsACKPacketString(char *buffer,struct HMI_InfoBase hsb)
{	int sizeofPacket = 0;
	
  sprintf(buffer,"%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%04X%02X\r",hsb.start,hsb.sender,
         hsb.packetLength,hsb.machineID,hsb.machineType,hsb.msgType,hsb.nextscreen,hsb.screen_substate,hsb.attributeCount,
         0,0,0,hsb.endChar);
  sizeofPacket = 29;
  return sizeofPacket;
	
}

int UpdateDiagPacketString(char *buffer,struct HMI_InfoBase hsb,struct HMI_DiagPacket hdp,int typeOfTest,int motorID,int signalVoltage,int rpm)
{    int sizeofPacket = 0;
    //tlv1-type of test,tlv2 - motorID .tlv3 - sigal voltage ,tlv4 - current trpm
		sprintf(buffer,"%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%04X%02X%02X%04X%02X%02X%04X%02X%02X%04X%02X\r",hsb.start,hsb.sender,hsb.packetLength,
		hsb.machineID,hsb.machineType,hsb.msgType,hsb.nextscreen,hsb.screen_substate,hsb.attributeCount,hdp.tlv1Code,
		hdp.tlv1length,typeOfTest,hdp.tlv2Code,hdp.tlv2length,motorID,hdp.tlv3Code,hdp.tlv3length,
    signalVoltage,hdp.tlv4Code,hdp.tlv4length,rpm,hsb.endChar);
    sizeofPacket = 53;
    
    return sizeofPacket;
}

int HMI_Get_DiagOver_PacketString(char *buffer,struct HMI_InfoBase hsb,char status)
{	int sizeofPacket = 0;
	
  sprintf(buffer,"%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%04X%02X\r",hsb.start,hsb.sender,
         hsb.packetLength,hsb.machineID,hsb.machineType,hsb.msgType,hsb.nextscreen,status,1,
         HMI_DIAG_END_OF_TEST_CODE,2,0,hsb.endChar);
  sizeofPacket = 29;
  return sizeofPacket;
	
}
//--------------------------HELPER FNS------------------------------------
unsigned char charToHexDigit(char c)
{
	if(islower(c)){
            /* Convert lower case character to upper case 
              using toupper function */
							c = toupper(c);
							} 
  if (c >= 'A')
    return c - 'A' + 10;
  else
    return c - '0';
}

unsigned int stringToINT(char c[4])
{
  return charToHexDigit(c[3]) + 16 * charToHexDigit(c[2]) + 256 * charToHexDigit(c[1])  + 4096 * charToHexDigit(c[0]);
}

unsigned int StringToHex(char c[2])
{
  return charToHexDigit(c[1]) + 16 * charToHexDigit(c[0]);

}

unsigned int StringDecodeAsInteger(char c[4])
{
  return charToHexDigit(c[3]) + charToHexDigit(c[2])*10 + charToHexDigit(c[1]) * 100 + charToHexDigit(c[0])*1000;

}

unsigned long stringToFLOAT(char c[8])
{
  return charToHexDigit(c[7]) + 16 * charToHexDigit(c[6]) + 256 * charToHexDigit(c[5])  + 4096 * charToHexDigit(c[4]) + 65536 * charToHexDigit(c[3]) + 1048576 * charToHexDigit(c[2]) + 16777216 * charToHexDigit(c[1])  +  268435456 * charToHexDigit(c[0]);
}
//----------------------------------------------------------------------------



char state;
int hex_msgInfo;
int hex_rtfUpdate;
char rtfUpdate [4];
char HMI_BasePacket_Decode(char *receiveBuffer)
{
	char machineID[2];
	char machineType[2];
	char msgInfo[2];
	char disable_start_msg [2];
	//settings
	char yarnCount[4];
	char outputYarnDia[8] ;
	char spindleSpeed [4];
	char tpi[4];
	char packageHeight[4];
	char diaBuildFactor [8];
	char windingOffsetCoils[8];
	char windingClosenessFactor[4];

	char motorID[4];
	char typeOfTest[4];
	char targetRPM[4];
	char targetSignalVoltage[4];
	char testTime[4];
	
	//decoded
	//char hex_machineID;
	//char hex_machineType;
	int hex_disable_start_msg;

	//settings
	unsigned int hex_yarnCount ;
	unsigned long hex_outputYarnDia ;
	unsigned int hexSpindleSpeed;
	unsigned int hex_tpi ;
	unsigned int hex_packageHeight ;
    unsigned long hex_diaBuildFactor ;
	unsigned int hex_windingClosenessFactor ;
	unsigned long hex_windingOffsetCoils;
	
	float outputYarnDia_F;
	float diaBuildFactor_F;
	float windingOffetCoils_F;

	//diag
	unsigned int	D_motorID ;
	unsigned int  D_typeOfTest ;
	unsigned int	D_targetRPM ;
	unsigned int	D_targetSignalVoltage ;
	unsigned int	D_testTime ;
	
	//start char will be 7E, dont take that out
	//right now we re getting:
	//7E02B0101029900010002007E  - > im paired MSG -> here we want the 99
	//7E020B010102030001010200017E  -> do not start packet -> here we want the 01/02/00 before the 7E(telling us which screen teh app is in)
	//7E0021010102040001102400DC4020000004DD4093333303203F99999A000C01C200787E -> settings change packet. the identifier is 
	// the 04, and then the vals come.

	strncpy(machineID,receiveBuffer+6,2);
	strncpy(machineType,receiveBuffer+8,2);
	strncpy(msgInfo,receiveBuffer+12,2);
	strncpy(disable_start_msg,receiveBuffer+24,2);
	strncpy(rtfUpdate,receiveBuffer+22,4);

	//hex_machineID = StringToHex(machineID);
	//hex_machineType = StringToHex(machineType);
	hex_msgInfo = StringToHex(msgInfo);
	hex_disable_start_msg = StringToHex(disable_start_msg);
	hex_rtfUpdate = stringToINT(rtfUpdate);
		
	if ((int)hex_msgInfo == FROM_HMI_IM_PAIRED)
		{
		state =1;
		return hex_msgInfo;
		}
	else if ((int)hex_msgInfo == FROM_HMI_DISABLE_MACHINE_START) //siable start can 0,1, or 2
		{
			state = 2;
		return hex_disable_start_msg;
		}
		
	 else if ((int)hex_msgInfo == FROM_HMI_ENABLE_MACHINE_START)  
		{
			state = 3;
		return hex_disable_start_msg;
		}
	else if ((int)hex_msgInfo == FROM_HMI_CHANGE_PROCESS_PARAMS)
		{
			state = 4;
			// update the update struct
			//flyer settings msg
			// 7E 01 25 01 03 02 04 00 01 60 24 0320 410ccccd 3f666666 3f2e147b 0041 0136 3f8ccccd 3f99999a 0030 7E

			//delivery speed comes as float, but we make it int
			strncpy(yarnCount,receiveBuffer+22,4);
			strncpy(outputYarnDia,receiveBuffer+26,8);
			strncpy(spindleSpeed,receiveBuffer+34,4);
			strncpy(tpi,receiveBuffer+38,4);
			strncpy(packageHeight,receiveBuffer+42,4);
			strncpy(diaBuildFactor,receiveBuffer+46,8);
			strncpy(windingClosenessFactor,receiveBuffer+54,4);
			strncpy(windingOffsetCoils,receiveBuffer+58,8);

			hex_yarnCount = stringToINT(yarnCount);
			hex_outputYarnDia = stringToFLOAT(outputYarnDia);
			hexSpindleSpeed = stringToINT(spindleSpeed);
			hex_tpi = stringToINT(tpi);
			hex_packageHeight = stringToINT(packageHeight);
			hex_diaBuildFactor = stringToFLOAT(diaBuildFactor);
			hex_windingClosenessFactor = stringToINT(windingClosenessFactor);
			hex_windingOffsetCoils = stringToFLOAT(windingOffsetCoils);

			outputYarnDia_F = *((float*)&hex_outputYarnDia);
			diaBuildFactor_F = *((float*)&hex_diaBuildFactor);
			windingOffetCoils_F = *((float*)&hex_windingOffsetCoils);

			//update ucsp
			msUpdate.inputYarnCountNe = hex_yarnCount;
			msUpdate.outputYarnDia = outputYarnDia_F;
			msUpdate.spindleSpeed = hexSpindleSpeed;
			msUpdate.tpi = hex_tpi;
			msUpdate.packageHeight = hex_packageHeight;
			msUpdate.diaBuildFactor = diaBuildFactor_F;
			msUpdate.windingClosenessFactor = hex_windingClosenessFactor;
			msUpdate.windingOffsetCoils = windingOffetCoils_F;
			
			return FROM_HMI_UPDATED_SETTINGS;
		}
	else if ((int)hex_msgInfo == FROM_HMI_DIAGNOSTIC_TEST)
	{
		state = 5;
		// Update the Diagnostics Struct
		//7E 02 55 01 01 02 05 00 05 01 02 0002 02 02 0005 03 02 0000 04 02 0078 05 02 0025 7E

		strncpy(typeOfTest,receiveBuffer+22,4);
		strncpy(motorID,receiveBuffer+30,4);
		
		// Here we need if we need to run te lift motor or not 
		D_typeOfTest = stringToINT(typeOfTest);
		D_motorID = StringDecodeAsInteger(motorID);
		//and put into the D struct
		D.typeofTest = D_typeOfTest;
		D.motorID = D_motorID;
				
		// if its the lift motor,then target signaVoltage == lift direction and
		// targetRpm == lift dist.test time is 0 and meaningless also.
		strncpy(targetSignalVoltage,receiveBuffer+38,4);
		strncpy(targetRPM,receiveBuffer+46,4);
		strncpy(testTime,receiveBuffer+54,4);

		D_targetSignalVoltage = stringToINT(targetSignalVoltage);
		D_targetRPM = stringToINT(targetRPM);
		D_testTime = stringToINT(testTime);
				
		if (((char)D_motorID == HMI_RD_LIFT_RIGHT) || ((char)D_motorID == HMI_RD_LIFT_LEFT))
		{
			D.liftDistance = D_targetRPM;
			D.liftDirection = D_targetSignalVoltage;
			D.targetSignal = 0;
			D.targetRPM = 0;
			D.testTime = 0;
			D.liftMode = 1;
			D.liftOver = 0;
			D.liftMotor = (char)D_motorID;
			// might need to move this later
			pulseCount1 = 0;//to ensure that we get the full distance
			pulseCount2 = 0;
		}
		else
		{		
				D.liftDistance = 0;
				D.liftDirection = 0;
				D.targetSignal = D_targetSignalVoltage;
				D.targetRPM = D_targetRPM;
				D.testTime = D_testTime;
				D.liftMode = 0;
			  D.liftOver = 0;
			  D.liftMotor = 0;
		}
		
		return FROM_DIAG_UPDATED_TEST_DETAILS;
	}
	
	else if (( int)hex_msgInfo == FROM_HMI_RTF_UPDATE) // if you get an RTF update msg
		{
			state = 5;
			if (S.current_state == HOMING_STATE)
			{
				return HOMING_RTF_CHANGE; 
			}
			else{
				if (hex_rtfUpdate == RTF_ADD)
							{ return ADD_TO_RTF;}
			
				else if (hex_rtfUpdate == RTF_SUBTRACT)
							{ return SUBTRACT_FROM_RTF;}
			
				else
						{ return NO_VAR; }
					}
		}
		
	else if	(( int)hex_msgInfo == FROM_HMI_NEW_DOFF){
					state = 6;
					return SIGNAL_NEW_DOFF;
				}
	
	else
		{
			state =6;
			return BAD_HMI_MSG;
		}
	
}

