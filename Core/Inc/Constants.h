#ifndef _CONSTANTS_H
#define _CONSTANTS_H

#define START_END_CHAR  126 // 0x7E
#define END_CR 10
#define SOURCE_ID  128 // 0x80
#define DEST_ID  255  // 0xFF
#define MACHINE_ID  1


#define SCNMSG 0x01
#define MC_SETTING_MSG 0x02
#define KEEPALIVE 0x03
#define HW_PARAMS_MSG 0x04
#define DIAG_MSG 0x07
#define ERROR_MSG 0x08
#define OUTPUT_PARAM_MSG 0x0A

#define MSP_PACKET_SIZE 11 // size of char array with only values till attribute cnt(including)

//pkt len when sending all param
#define CARDING_ALLPARAMS_PKT_LEN 38
#define DF_ALLPARAMS_PKT_LEN 22
#define FF_ALLPARAMS_PKT_LEN 42
//param code
#define CARDING_ALLPARAMS_CODE 0x10
#define DF_ALLPARAMS_CODE 0x40
#define FF_ALLPARAMS_CODE 0x60
//TLV-L val
#define CARDING_ALL_PARAM_LEN 24
#define DF_ALL_PARAM_LEN 8
#define FF_ALL_PARAM_LEN 28

#define NO_MOTOR 0x00

//DIAGNOSTIC ATTRIBUTES
#define DIAG_OPENLOOP 0x00
#define DIAG_CLOSEDLOOP 0x01
#define DIAG_ATTR_MOTORID 0x01
#define DIAG_ATTR_SIGNAL_IP_PERCENT 0x02
#define DIAG_ATTR_TARGET_RPM 0x03
#define DIAG_ATTR_TEST_TIME 0x04
#define DIAG_ATTR_TEST_RESULT 0x05

//ERROR ATTRIBUTES
#define ERROR_ATTR_MOTORID 0x01



#define CARDING_DELIVERY_SPEED_CODE  0x01//int
#define CARDING_TENSION_DRAFT_CODE  0x02//float
#define CARDING_CYL_SPEED_CODE  0x03//int
#define CARDING_CYL_FEED_CODE  0x04 // float
#define CARDING_BEATER_SPEED_CODE  0x05//int
#define CARDING_BEATER_FEED_CODE 0x06//float
#define CARDING_CONVEYOR_SPEED_CODE 0x07//inr
#define CARDING_CONVEYOR_DELAY_CODE 0x08//int
#define CARDING_CONVEYOR_DWELL_CODE 0x09 //int

#define DF_DELIVERY_SPEED_CODE 0x30 // int
#define DF_DRAFT_CODE  0x31  // float
#define DF_LENGTH_LIMIT_CODE 0x32 // int

#define FF_SPINDLE_SPEED_CODE 0x50 //int
#define FF_DRAFT_CODE 0x51 // float
#define FF_TPI_CODE 0x52  // float
#define FF_RTF_CODE 0x53  // float
#define FF_MAX_ROVING_LAYERS_CODE 0x54 // int
#define FF_MAX_HT_CODE 0x55 //int
#define FF_ROVING_WDTH_CODE 0x56 // float
#define FF_DELTA_BOBBIN_DIA_CODE 0x57 // float
#define FF_BOBBIN_DIA_CODE 0x58 //int

//Output Params Codes
#define PRODUCTION 0x01 // float

#endif
