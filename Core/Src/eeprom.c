
#include "eeprom.h"
#include "Structs.h"
#include "stm32f4xx_hal.h"

extern int chaseCount;
extern int pulseCount1;
extern int pulseCount2;
extern int directionChange;
extern int directionChange1;

extern I2C_HandleTypeDef hi2c3;

void EepromWriteInt(unsigned position,unsigned int data)
{
	int count = 0;
	if (data>255)
	{
		while(data>255)
		{
			data = data-255;
			count++;
		}
	}
	
	if(count>0)
	{
		HAL_I2C_Mem_Write(&hi2c3,EEPROM_ADDRESS, position,0xFF,(uint8_t*)&count,1,1);
		HAL_Delay(5);
		HAL_I2C_Mem_Write(&hi2c3,EEPROM_ADDRESS, position+1,0xFF,(uint8_t*)&data,1,1);
		HAL_Delay(5);
	}
	else
	{	
		HAL_I2C_Mem_Write(&hi2c3,EEPROM_ADDRESS, position,0xFF,(uint8_t*)&count,1,1);
		HAL_Delay(5);
		HAL_I2C_Mem_Write(&hi2c3,EEPROM_ADDRESS, position+1,0xFF,(uint8_t*)&data,1,1);
		HAL_Delay(5);
	}

}

unsigned int EepromReadInt(unsigned position)
{
	int count =0;
	unsigned int data = 0;
	HAL_I2C_Mem_Read(&hi2c3,EEPROM_ADDRESS,position,0xFF,(uint8_t*)&count,1,1);
	HAL_I2C_Mem_Read(&hi2c3,EEPROM_ADDRESS,position+1,0xFF,(uint8_t*)&data,1,1);
	data = data+(count*255);
	return data;
}


void EepromWriteFloat(unsigned position,float data)
{
	unsigned int data_in = 0;
	int count3 = 0;
	int count2 = 0;
	int count1 = 0;
	data_in =  *(unsigned int*)&data;
	if (data_in>16777216)
	{
		while(data_in>16777216)
		{
			data_in = data_in-16777216;
			count3++;
		}
	}
		if (data_in>65536)
	{
		while(data_in>65536)
		{
			data_in = data_in-65536;
			count2++;
		}
	}
	if (data_in>255)
	{
		while(data_in>255)
		{
			data_in = data_in-255;
			count1++;
		}
	}
	
	EepromWriteInt(position,count3);
	EepromWriteInt(position+2,count2);
	EepromWriteInt(position+4,count1);
	EepromWriteInt(position+6,data_in);
}


unsigned int EepromReadFloat(unsigned position)
{
	long data = 0;
	unsigned int count3_in = 0;
	unsigned int count2_in = 0;
	unsigned int count1_in = 0;
	unsigned int count0_in = 0;
	count3_in = EepromReadInt(position);
	count2_in = EepromReadInt(position+2);
	count1_in = EepromReadInt(position+4);
	count0_in = EepromReadInt(position+6);
	data = count0_in+(count1_in*255)+(count2_in*65536)+(count3_in*16777216);
	return data;  //data has to be converted to float use this *(float*)&data
}
/*
void ReadCardingSettingsFromEeprom(void)
{
	long data_out = 0;
	csp.deliverySpeed = EepromReadInt(DELIVERY_SPEED_ADDR);
	data_out = EepromReadInt(TENSION_DRAFT_ADDR);
	csp.tensionDraft= ((float)data_out)/(float)100;
	csp.cylinderSpeed = EepromReadInt(CYLINDER_SPEED_ADDR);
	data_out = EepromReadInt(CYLINDER_FEED_ADDR);
	csp.cylinderFeed= ((float)data_out)/(float)100;
	csp.beaterSpeed = EepromReadInt(BEATER_SPEED_ADDR);
	data_out = EepromReadInt(BEATER_FEED_ADDR);
	csp.beaterFeed = ((float)data_out)/(float)100;
	csp.conveyorSpeed = EepromReadInt(CONVEYOR_SPEED_ADDR);
	csp.conveyorDelay = EepromReadInt(CONVEYOR_DELAY_ADDR);
	csp.conveyorDwell = EepromReadInt(CONVEYOR_DWELL_ADDR);
	
}

void WriteCardingSettingsIntoEeprom(void)
{

	EepromWriteInt(DELIVERY_SPEED_ADDR,csp.deliverySpeed);
	EepromWriteInt(TENSION_DRAFT_ADDR,(int)(csp.tensionDraft*100));
	EepromWriteInt(CYLINDER_SPEED_ADDR,csp.cylinderSpeed);
	EepromWriteInt(CYLINDER_FEED_ADDR,(int)(csp.cylinderFeed*100));
	EepromWriteInt(BEATER_SPEED_ADDR,csp.beaterSpeed);
	EepromWriteInt(BEATER_FEED_ADDR,(int)(csp.beaterFeed*100));
	EepromWriteInt(CONVEYOR_SPEED_ADDR,csp.conveyorSpeed);
	EepromWriteInt(CONVEYOR_DELAY_ADDR,csp.conveyorDelay);
	EepromWriteInt(CONVEYOR_DWELL_ADDR,csp.conveyorDwell);

}


void ReadDrawFrameSettingsFromEeprom(void)
{
	long data_out = 0;
	dsp.deliverySpeed = EepromReadInt(DF_DELIVERY_SPEED_ADDR);
	data_out = EepromReadInt(DF_TENSION_DRAFT_ADDR);
	dsp.tensionDraft= ((float)data_out)/(float)100;
	dsp.lengthLimit = EepromReadInt(DF_LENGTHLIMIT_ADDR);
}

void WriteDrawFrameSettingsIntoEeprom(void)
{

	EepromWriteInt(DF_DELIVERY_SPEED_ADDR,dsp.deliverySpeed);
	EepromWriteInt(DF_TENSION_DRAFT_ADDR,(int)(dsp.tensionDraft*100));
	EepromWriteInt(DF_LENGTHLIMIT_ADDR,dsp.lengthLimit);

}
*/

void ReadFlyerSettingsFromEeprom(void)
{
}

void ReadRFSettingsFromEeprom(void)
{
	long data_out = 0;
	fsp.spindleSpeed = EepromReadInt(RF_SPINDLE_SPEED_ADDR);
	
	fsp.draft = EepromReadInt(RF_TENSION_DRAFT_ADDR);
	
	fsp.tpi = EepromReadInt(RF_TPI_ADDR);

	fsp.yarnCount = EepromReadInt(RF_YARN_COUNT);
	
	data_out = EepromReadInt(RF_BIND_WIND_RATIO);
	fsp.bindWindRatio = ((float)data_out)/(float)100;
	
	data_out = EepromReadInt(RF_CHASE_LENGTH);
	fsp.chaseLength= ((float)data_out)/(float)100;
	
	fsp.preferredPackageSize = EepromReadInt(RF_PREFERRED_PKG_SIZE);
	
	fsp.rightSideOn = EepromReadInt(RF_RIGHT_ON);
	
	fsp.leftSideOn= EepromReadInt(RF_LEFT_ON);	

}

void WriteFlyerSettingsIntoEeprom(void)
{

}

void WriteRFSettingsIntoEeprom(void)
{
	EepromWriteInt(RF_YARN_COUNT,fsp.yarnCount);
	EepromWriteInt(RF_SPINDLE_SPEED_ADDR,fsp.spindleSpeed);
	EepromWriteInt(RF_TENSION_DRAFT_ADDR,fsp.draft);
	EepromWriteInt(RF_TPI_ADDR,fsp.tpi);
	EepromWriteInt(RF_BIND_WIND_RATIO,(int)(fsp.bindWindRatio*100));
	EepromWriteInt(RF_CHASE_LENGTH,(int)(fsp.chaseLength*100));
	EepromWriteInt(RF_PREFERRED_PKG_SIZE,fsp.preferredPackageSize);
	EepromWriteInt(RF_RIGHT_ON,fsp.rightSideOn);
	EepromWriteInt(RF_LEFT_ON,fsp.leftSideOn);
}


void ReadOldDoffSettingsFromEeprom(void)
{
	chaseCount = EepromReadInt(RF_CURRENT_CHASE_COUNT);
	pulseCount1 = EepromReadInt(RF_CURRENT_PULSECOUNT1);
	pulseCount2 = EepromReadInt(RF_CURRENT_PULSECOUNT2);
	directionChange = EepromReadInt(RF_CURRENT_DIRECTION_RIGHT);
	if(directionChange == 255){
		directionChange = -1;
	}
	directionChange1 = EepromReadInt(RF_CURRENT_DIRECTION_LEFT);
	if(directionChange1 == 255){
		directionChange1 = -1;
	}
	S.savedStateExists = EepromReadInt(RF_SAVED_SETTINGS);
}
