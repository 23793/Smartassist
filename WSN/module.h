#ifndef _APP_H
#define _APP_H

#define LEDWHITE	OCR3B // oder PE4
#define LEDRED		PE3
#define LEDBLUE		PE2
#define FAN			PE7 // oder OCR3A

#define turnOff(PIN)	(PORTE &= ~(1<<PIN))
#define turnOn(PIN)		(PORTE |= (1<<PIN))
#define isOn(PIN)		(PINE & (1<<PIN))
#define isOff(PIN)		(!(PINE & (1<<PIN)))

// Endpoints Server
#define srcTemperature_Measurement_Server 1
#define srcIlluminance_Measurement_Server 2
#define srcFan_Conrol_Server 3
#define srcOnOff_Light_Server 4
#define srcOnOff_COOLING_Server 6
#define srcOnOff_HEATING_Server 7
#define srcApp_OnOff_Light_Server 9
#define srcOnOff_Status_Server 10
#define srcOnOff_Mode_Climate_Server 11
#define srcOnOff_Mode_Light_Server 12
#define srcTemperature_Zielwert 13
#define srcIlluminance_Zielwert 14

// Endpoints Client
#define srcOnOff_Light_Client 8

// Endpoints Destination

#define dstTemperature_Measurement_Client 1
#define dstIlluminance_Measurement_Client 4
#define dstOnOff_Light_Client 2
#define dstOnOff_Status_Client 3
#define dstOnOff_Mode_Climate 5
#define dstOnOff_Mode_Light_Client 6



//Addresse TemperatureSensor
#define LM73_DEVICE_ADDRESS 0x4D

// Variablen f?r Temperaturmessung
#define TEMPERATURE_MEASUREMENT_MEASURED_VALUE_ATTRIBUTE_VALUE       0x0000
#define TEMPERATURE_MEASUREMENT_MIN_MEASURED_VALUE_ATTRIBUTE_VALUE   0x0000
#define TEMPERATURE_MEASUREMENT_MAX_MEASURED_VALUE_ATTRIBUTE_VALUE   0x0000
#define TEMPERATURE_MEASUREMENT_TOLERANCE_ATTRIBUTE_VALUE            0x0000
#define TEMPERATURE_MEASUREMENT_MEASURED_VALUE_PERIODIC_CHANGE       1000

// Variablen f?r Helligkeitsmessung
#define ILLUMINANCE_MEASUREMENT_MEASURED_VALUE_ATTRIBUTE_VALUE       0x0000
#define ILLUMINANCE_MEASUREMENT_MIN_MEASURED_VALUE_ATTRIBUTE_VALUE   0x0000
#define ILLUMINANCE_MEASUREMENT_MAX_MEASURED_VALUE_ATTRIBUTE_VALUE   0x0000
#define ILLUMINANCE_MEASUREMENT_TOLERANCE_ATTRIBUTE_VALUE            0x0000

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
	START_NETWORK,
	REG_ENDPOINT,
	NOTHING
} AppState_t;
#endif
// eof app.h
