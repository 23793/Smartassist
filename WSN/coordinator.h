
#ifndef _SCHALTER_H
#define _SCHALTER_H

// Endpoints
#define srcTemperature_Measurement_Client1 1
#define srcTemperature_Measurement_Client2 2
#define srcTemperature_Measurement_Client3 3
#define srcIlluminance_Measurement_Client1 4
#define srcIlluminance_Measurement_Client2 5
#define srcIlluminance_Measurement_Client3 6
#define srcOnOff_Light_Client1 7
#define srcOnOff_Light_Client2 8
#define srcOnOff_Light_Client3 9
#define srcOnOff_Status_Client1 10
#define srcOnOff_Status_Client2 11
#define srcOnOff_Status_Client3 12
#define srcOnOff_Mode_Climate_Client1 13
#define srcOnOff_Mode_Climate_Client2 14
#define srcOnOff_Mode_Climate_Client3 15
#define srcOnOff_Mode_Light_Client1 16
#define srcOnOff_Mode_Light_Client2 17
#define srcOnOff_Mode_Light_Client3 18
#define srcTemperature_Zielwert_Client1 19
#define srcTemperature_Zielwert_Client2 20
#define srcTemperature_Zielwert_Client3 21
#define srcIlluminance_Zielwert_Client1 25
#define srcIlluminance_Zielwert_Client2 26
#define srcIlluminance_Zielwert_Client3 50

#define dstTemperature_Zielwert_Server1 19
#define dstTemperature_Zielwert_Server2 20
#define dstTemperature_Zielwert_Server3 21
#define dstIlluminance_Zielwert_Server1 25
#define dstIlluminance_Zielwert_Server2 26
#define dstIlluminance_Zielwert_Server3 50
#define dstApp_OnOff_Light_Server1 31
#define dstApp_OnOff_Light_Server2 31
#define dstApp_OnOff_Light_Server3 31
#define dstOnOff_Mode_Light_Server1 34
#define dstOnOff_Mode_Light_Server2 34
#define dstOnOff_Mode_Light_Server3 34
#define dstOnOff_Mode_Climate_Server1 37
#define dstOnOff_Mode_Climate_Server2 37
#define dstOnOff_Mode_Climate_Server3 37
#define dstOnOff_Status_Server1 40
#define dstOnOff_Status_Server2 40
#define dstOnOff_Status_Server3 40


typedef struct{
	uint8_t ID;
	bool status;						// 1==active, 0 == inactive
	bool mode_light;					// 1 == automatisch, 0==manuell
	bool mode_climate;					// 1 == automatisch, 0==manuell
	bool LEDWHITE_status;				// 1 == an, 0==aus
	uint16_t illuminanceValue;			// 0 - 255
	uint16_t temperatureValue;			// VVNN (V= Vorkomma, N= Nachkomma)
	uint16_t illuminanceReference;		// 0 - 255
	uint16_t temperatureReference;		// VVNN (V= Vorkomma, N= Nachkomma)


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
	DATA_PROCESS,
	NOTHING
} AppState_t;
#endif
