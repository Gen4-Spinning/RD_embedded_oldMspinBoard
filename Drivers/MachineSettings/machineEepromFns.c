/*
 * EepromFns.c
 *
 *  Created on: 06-Mar-2023
 *      Author: harsha
 */

/*
#include "Eeprom.h"
#include "machineSettings.h"
#include "machineEepromSettings.h"

void ReadMachineSettingsFromEeprom(machineSettingsTypeDef *m){
	m->inputYarnCountNe = EE_ReadInteger(RD_INPUT_YARN_COUNT_NE_ADDR);
	m->outputYarnDia  = EE_ReadFloat(RD_OUTPUT_YARN_DIA_ADDR);
	m->spindleSpeed = EE_ReadInteger(RD_SPINDLESPEED_ADDR);
	m->tpi = EE_ReadInteger(RD_TPI_ADDR);
	m->packageHeight = EE_ReadInteger(RD_PACKAGE_HEIGHT_ADDR);
	m->windingClosenessFactor = EE_ReadInteger(RD_WINDINGCLOSENESSFACTOR_ADDR);
	m->windingOffsetCoils = EE_ReadInteger(RD_WINDINGOFFSET_COILS_ADDR);
	m->diaBuildFactor = EE_ReadFloat(RD_DIA_BUILD_FACTOR_ADDR);
}


uint8_t WriteMachineSettingsIntoEeprom(machineSettingsTypeDef *m){
	uint8_t dataWritten = 0;
    dataWritten += EE_WriteInteger(m->inputYarnCountNe,RD_INPUT_YARN_COUNT_NE_ADDR);
    HAL_Delay(2);
    dataWritten += EE_WriteFloat(m->outputYarnDia,RD_OUTPUT_YARN_DIA_ADDR);
    HAL_Delay(2);
    dataWritten += EE_WriteInteger(m->spindleSpeed,RD_SPINDLESPEED_ADDR);
    HAL_Delay(2);
    dataWritten += EE_WriteInteger(m->tpi,RD_TPI_ADDR);
    HAL_Delay(2);
    dataWritten += EE_WriteInteger(m->packageHeight,RD_PACKAGE_HEIGHT_ADDR);
    HAL_Delay(2);
    dataWritten += EE_WriteInteger(m->windingClosenessFactor,RD_WINDINGCLOSENESSFACTOR_ADDR);
    HAL_Delay(2);
    dataWritten += EE_WriteInteger(m->windingOffsetCoils,RD_WINDINGOFFSET_COILS_ADDR);
    HAL_Delay(2);
    dataWritten += EE_WriteFloat(m->diaBuildFactor,RD_DIA_BUILD_FACTOR_ADDR);
    HAL_Delay(2);
    if (dataWritten == 8)
    	{return 0;}
    else{
    	return 1;}

}


uint8_t CheckMachineSettings(machineSettingsTypeDef* m){
	//typically when something goes wrong with the eeprom you get a value that is very high..
	//to allow for changes place to place without changing this code, we just set the thresholds to  2* maxRange.
	// dont expect in any place the nos to go higher than this..NEED TO PUT LOWER BOUNDS FOR EVERYTHING
	if ((m->inputYarnCountNe > 80 ) || (m->inputYarnCountNe < 10)){
		return 0;
	}
	if ((m->outputYarnDia > 1.5f) || (m->outputYarnDia < 0.05f)){
		return 0;
	}
	if ((m->tpi > 30) || (m->tpi < 10)){
		return 0;
	}
	if ((m->spindleSpeed > 12000) || (m->spindleSpeed < 6000)){
		return 0;
	}
	if (m->packageHeight > 250 || (m->packageHeight < 50)){
		return 0;
	}
	if ((m->windingClosenessFactor > 200) || (m->windingClosenessFactor < 50)){
		return 0;
	}
	if ((m->windingOffsetCoils > 5) || (m->windingOffsetCoils < 1)){
		return 0;
	}
	if ((m->diaBuildFactor > 2.5) || (m->diaBuildFactor < 0.05)){
		return 0;
	}
	return 1;
}

void LoadDefaultMachineSettings(machineSettingsTypeDef* m){
	m->inputYarnCountNe = DEFAULT_INPUT_YARN_COUNT_NE;
	m->outputYarnDia = DEFAULT_OUTPUT_YARN_DIA;
	m->spindleSpeed = DEFAULT_SPINDLESPEED;
	m->tpi = DEFAULT_TPI;
	m->packageHeight = DEFAULT_PACKAGE_HEIGHT;
	m->windingClosenessFactor = DEFAULT_WINDINGCLOSENESSFACTOR;
	m->windingOffsetCoils = DEFAULT_WINDINGOFFSET_COILS;
	m->diaBuildFactor = DEFAULT_DIA_BUILD_FACTOR;
}
*/
