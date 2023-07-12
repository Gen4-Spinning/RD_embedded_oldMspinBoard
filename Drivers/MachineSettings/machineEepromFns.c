/*
 * EepromFns.c
 *
 *  Created on: 06-Mar-2023
 *      Author: harsha
 */


#include "eeprom.h"
#include "machineControl.h"
#include "machineEepromSettings.h"

void ReadMachineSettingsFromEeprom(machineSettingsTypeDef *m){
	m->inputYarnCountNe = EepromReadInt(RD_INPUT_YARN_COUNT_NE_ADDR);
	m->outputYarnDia  = EepromReadInt(RD_OUTPUT_YARN_DIA_ADDR)/100.0;
	m->spindleSpeed = EepromReadInt(RD_SPINDLESPEED_ADDR);
	m->tpi = EepromReadInt(RD_TPI_ADDR);
	m->packageHeight = EepromReadInt(RD_PACKAGE_HEIGHT_ADDR);
	m->windingClosenessFactor = EepromReadInt(RD_WINDINGCLOSENESSFACTOR_ADDR);
	m->windingOffsetCoils = EepromReadInt(RD_WINDINGOFFSET_COILS_ADDR)/100.0;
	m->diaBuildFactor = EepromReadInt(RD_DIA_BUILD_FACTOR_ADDR)/100.0;
}


uint8_t WriteMachineSettingsIntoEeprom(machineSettingsTypeDef *m){
	uint8_t dataWritten = 0;
    dataWritten += EepromWriteInt2(RD_INPUT_YARN_COUNT_NE_ADDR,m->inputYarnCountNe);
    HAL_Delay(2);
    dataWritten += EepromWriteInt2(RD_OUTPUT_YARN_DIA_ADDR,(uint16_t)(m->outputYarnDia*100));
    HAL_Delay(2);
    dataWritten += EepromWriteInt2(RD_SPINDLESPEED_ADDR,m->spindleSpeed);
    HAL_Delay(2);
    dataWritten += EepromWriteInt2(RD_TPI_ADDR,m->tpi);
    HAL_Delay(2);
    dataWritten += EepromWriteInt2(RD_PACKAGE_HEIGHT_ADDR,m->packageHeight);
    HAL_Delay(2);
    dataWritten += EepromWriteInt2(RD_WINDINGCLOSENESSFACTOR_ADDR,m->windingClosenessFactor);
    HAL_Delay(2);
    dataWritten += EepromWriteInt2(RD_WINDINGOFFSET_COILS_ADDR,(uint16_t)(m->windingOffsetCoils * 100));
    HAL_Delay(2);
    dataWritten += EepromWriteInt2(RD_DIA_BUILD_FACTOR_ADDR,(uint16_t)(m->diaBuildFactor * 100));
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
	if ((m->spindleSpeed != 6000) && (m->spindleSpeed !=  8000) && (m->spindleSpeed !=  9000) && (m->spindleSpeed !=  10000)){
		return 0;
	}
	if (m->packageHeight > 200 || (m->packageHeight < 50)){
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

