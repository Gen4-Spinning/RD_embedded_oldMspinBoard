#ifndef __EEPROM_H
#define __EEPROM_H

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

#define RF_SPINDLE_SPEED_ADDR 0X40
#define RF_TENSION_DRAFT_ADDR 0X44
#define RF_TPI_ADDR 0X48
#define RF_YARN_COUNT 0X4C
#define RF_BIND_WIND_RATIO 0X50
#define RF_CHASE_LENGTH 0X54
#define RF_PREFERRED_PKG_SIZE 0X58	
#define RF_RIGHT_ON 0X5C	
#define RF_LEFT_ON 0X60


#define RF_CURRENT_CHASE_COUNT 0X70
#define RF_CURRENT_PULSECOUNT1 0X72
#define RF_CURRENT_PULSECOUNT2 0X74
#define RF_CURRENT_DIRECTION_LEFT 0X78
#define RF_CURRENT_DIRECTION_RIGHT 0X82
#define RF_SAVED_SETTINGS 0X86

void EepromWriteInt(unsigned position,unsigned int data);
unsigned int EepromReadInt(unsigned position);
void EepromWriteFloat(unsigned position,float data);
unsigned int EepromReadFloat(unsigned position);
void WriteCardingSettingsIntoEeprom(void);
void ReadCardingSettingsFromEeprom(void);
void WriteDrawFrameSettingsIntoEeprom(void);
void ReadDrawFrameSettingsFromEeprom(void);
void WriteFlyerSettingsIntoEeprom(void);
void ReadFlyerSettingsFromEeprom(void);
void WriteRFSettingsIntoEeprom(void);
void ReadRFSettingsFromEeprom(void);
void ReadOldDoffSettingsFromEeprom(void);
#endif /* __ENCODER_H */

