#ifndef __EEPROM_H
#define __EEPROM_H

#include "stdio.h"

#define EEPROM_ADDRESS 0xAC

#define DELIVERY_SPEED_ADDR 0X00
#define TENSION_DRAFT_ADDR 0X02
#define CYLINDER_SPEED_ADDR 0X06
#define CYLINDER_FEED_ADDR 0X08
#define BEATER_SPEED_ADDR 0X0C
#define BEATER_FEED_ADDR 0X0E
#define CONVEYOR_SPEED_ADDR 0X12
#define CONVEYOR_DELAY_ADDR 0X14
#define CONVEYOR_DWELL_ADDR 0X16


#define DF_DELIVERY_SPEED_ADDR 0X20
#define DF_TENSION_DRAFT_ADDR 0X22
#define DF_LENGTHLIMIT_ADDR 0X26

#define FF_SPINDLE_SPEED_ADDR 0X30
#define FF_TENSION_DRAFT_ADDR 0X32
#define FF_TPI_ADDR 0X36
#define FF_RTF_ADDR 0X3A
#define FF_MAX_ROVING_LAYERS_ADDR 0X3E
#define FF_BOBBIN_HEIGHT_ADDR 0X40
#define FF_ROVING_WIDTH_ADDR 0X42
#define FF_DELTA_BOBBIN_DIA_ADDR 0X46
#define FF_BARE_BOBBIN_DIA_ADDR 0X4B
#define FF_BOOSTFACTOR_ADDR 0X4D
#define FF_BUCKFACTOR_ADDR 0X50


void EepromWriteInt(unsigned position,unsigned int data);
uint8_t EepromWriteInt2(unsigned position,unsigned int data);

void EepromWriteFloat(unsigned position,float data);
uint8_t EepromWriteFloat2(unsigned position,float data);

unsigned int EepromReadInt(unsigned position);
unsigned int EepromReadFloat(unsigned position);

#endif /* __ENCODER_H */

