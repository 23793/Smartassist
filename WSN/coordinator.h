
#ifndef _SCHALTER_H
#define _SCHALTER_H

// Endpoints
#define srcTemperature_Measurement_Client 1
#define srcIlluminance_Measurement_Client 4
#define srcOnOff_Light_Client 2
#define srcOnOff_Status_Client 3
#define srcOnOff_Mode_Climate_Client 5
#define srcOnOff_Mode_Light_Client 6

typedef struct{
	uint64_t ID;
	bool status;		// 1==active, 0 == inactive
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
