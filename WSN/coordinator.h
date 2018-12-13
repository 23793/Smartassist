
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
#define srcOnOff_Status_Client2 13
#define srcOnOff_Status_Client3 14
#define srcOnOff_Mode_Climate_Client1 5
#define srcOnOff_Mode_Climate_Client2 15
#define srcOnOff_Mode_Climate_Client3 16
#define srcOnOff_Mode_Light_Client1 6
#define srcOnOff_Mode_Light_Client2 17
#define srcOnOff_Mode_Light_Client3 18

typedef struct{
	uint64_t ID;
	bool status;				// 1==active, 0 == inactive
	bool mode_light;			// 1 == automatisch, 0==manuell
	bool mode_climate;
	bool LEDWHITE_status;
	uint16_t illuminanceValue;
	uint16_t illuminanceReference;
	double temperatureValue;
	double temperatureReference;
	uint8_t LEDWHITE_power;
	uint8_t LEDWHITE_duty;		// Wert zwischen 0 und 255 (0 = 0%, 255 = 100%)
	bool LEDBLUE_status;
	bool LEDRED_status;
	bool FAN_status;
} Module;

typedef enum{
	INIT,
	JOIN_NETWORK,
	NOTHING
} AppState_t;
#endif
