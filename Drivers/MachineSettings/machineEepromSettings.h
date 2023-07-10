/*
 * EepromSettings.h
 *
 *  Created on: 06-Mar-2023
 *      Author: harsha
 */

#ifndef INC_EEPROMSETTINGS_H_
#define INC_EEPROMSETTINGS_H_

//Addresses
//Dont let a address go across address 32 and its multiples. Thats one page.
#define RD_INPUT_YARN_COUNT_NE_ADDR 0X02 //int
#define RD_OUTPUT_YARN_DIA_ADDR 0X04 // float
#define RD_SPINDLESPEED_ADDR 0X08 //int
#define RD_TPI_ADDR 0X0A //int
#define RD_PACKAGE_HEIGHT_ADDR 0X0C //int
#define RD_DIA_BUILD_FACTOR_ADDR 0X10 //float
#define RD_WINDINGOFFSET_COILS_ADDR 0X14 //int
#define RD_WINDINGCLOSENESSFACTOR_ADDR 0X16 //int

//DEFAULTS
#define DEFAULT_INPUT_YARN_COUNT_NE 20
#define DEFAULT_OUTPUT_YARN_DIA 0.4
#define DEFAULT_SPINDLESPEED 8000
#define DEFAULT_TPI 14
#define DEFAULT_PACKAGE_HEIGHT 200
#define DEFAULT_DIA_BUILD_FACTOR 0.12
#define DEFAULT_WINDINGOFFSET_COILS 2
#define DEFAULT_WINDINGCLOSENESSFACTOR 108


#endif /* INC_EEPROMSETTINGS_H_ */
