
#ifndef _SCHALTER_H
#define _SCHALTER_H

// Endpoints
#define srcTemperature_Measurement_Client1 1
#define srcTemperature_Measurement_Client2 7
#define srcTemperature_Measurement_Client3 8
#define srcIlluminance_Measurement_Client1 4
#define srcIlluminance_Measurement_Client2 9
#define srcIlluminance_Measurement_Client3 10
#define srcOnOff_Light_Client1 2
#define srcOnOff_Light_Client2 11
#define srcOnOff_Light_Client3 12
#define srcOnOff_Status_Client1 3
#define srcOnOff_Status_Client2 19
#define srcOnOff_Status_Client3 14
#define srcOnOff_Mode_Climate_Client1 5
#define srcOnOff_Mode_Climate_Client2 15
#define srcOnOff_Mode_Climate_Client3 16
#define srcOnOff_Mode_Light_Client1 6
#define srcOnOff_Mode_Light_Client2 17
#define srcOnOff_Mode_Light_Client3 18

#define dstTemperature_Zielwert_Server1 33
#define dstTemperature_Zielwert_Server2 34
#define dstTemperature_Zielwert_Server3 35
#define srcTemperature_Zielwert_Client1 33
#define srcTemperature_Zielwert_Client2 34
#define srcTemperature_Zielwert_Client3 35

#define dstIlluminance_Zielwert_Server1 50
#define dstIlluminance_Zielwert_Server2 51
#define dstIlluminance_Zielwert_Server3 52
#define srcIlluminance_Zielwert_Client1 50
#define srcIlluminance_Zielwert_Client2 54
#define srcIlluminance_Zielwert_Client3 55

#define dstApp_OnOff_Light_Server1 9
#define dstApp_OnOff_Light_Server2 40
#define dstApp_OnOff_Light_Server3 41

#define dstOnOff_Mode_Light_Server1 12
#define dstOnOff_Mode_Light_Server2 22
#define dstOnOff_Mode_Light_Server3 22
#define dstOnOff_Mode_Climate_Server1 11
#define dstOnOff_Mode_Climate_Server2 22
#define dstOnOff_Mode_Climate_Server3 22
#define dstOnOff_Status_Server1 10
#define dstOnOff_Status_Server2 22
#define dstOnOff_Status_Server3 22
#define dstOnOff_Light_Server1 12
#define dstOnOff_Light_Server2 22
#define dstOnOff_Light_Server3 22

typedef struct{
	uint8_t ID;
	bool status;				// 1==active, 0 == inactive
	bool mode_light;			// 1 == automatisch, 0==manuell
	bool mode_climate;
	bool LEDWHITE_status;
	uint16_t illuminanceValue;
	uint16_t illuminanceReference;
	uint16_t temperatureValue;
	uint16_t temperatureReference;
	uint8_t LEDWHITE_power;
	uint8_t LEDWHITE_duty;		// Wert zwischen 0 und 255 (0 = 0%, 255 = 100%)
	bool LEDBLUE_status;
	bool LEDRED_status;
	bool FAN_status;
} Module;

// Struct um Daten Temperatur Zielwerte zu empfangen
BEGIN_PACK
typedef struct _AppMessage_t_Temperature{
	uint8_t header[APS_ASDU_OFFSET];
	uint8_t data[4];
	uint8_t footer[APS_AFFIX_LENGTH - APS_ASDU_OFFSET];
} PACK AppMessage_t_Temperature;
END_PACK

// Struct um Daten illuminance Zielwerte zu empfangen
BEGIN_PACK
typedef struct _AppMessage_t_Illuminance{
	uint8_t header[APS_ASDU_OFFSET];
	uint8_t data[3];
	uint8_t footer[APS_AFFIX_LENGTH - APS_ASDU_OFFSET];
} PACK AppMessage_t_Illuminance;
END_PACK


typedef enum{
	INIT,
	JOIN_NETWORK,
	NOTHING
} AppState_t;
#endif
