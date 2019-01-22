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
#define srcFan_Conrol_Server 100
#define srcOnOff_Light_Server 4
#define srcOnOff_COOLING_Server 101
#define srcOnOff_HEATING_Server 102
#define srcApp_OnOff_Light_Server 31
#define srcOnOff_Status_Server 40
#define srcOnOff_Mode_Climate_Server 37
#define srcOnOff_Mode_Light_Server 34
#define srcTemperature_Zielwert 21
#define srcIlluminance_Zielwert 50

// Endpoints Client
#define srcOnOff_Light_Client 8

// Endpoints Destination

#define dstTemperature_Measurement_Client 3
#define dstIlluminance_Measurement_Client 6
#define dstOnOff_Light_Client 9
#define dstOnOff_Status_Client 12
#define dstOnOff_Mode_Climate_Client 15
#define dstOnOff_Mode_Light_Client 18
#define dstOnOff_Cooling_Client 30
#define dstOnOff_Heating_Client 31

#define MODULE_ID 3
#define STATUS_ATTRIBUTE_BUFFER_SIZE 10
#define MODELIGHT_ATTRIBUTE_BUFFER_SIZE 11
#define MODECLIMATE_ATTRIBUTE_BUFFER_SIZE 12
#define LIGHT_ATTRIBUTE_BUFFER_SIZE 13
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
	uint8_t ID;
	uint16_t illuminanceReference;
	int16_t temperatureReference;
	uint8_t LEDWHITE_power;
	uint8_t LEDWHITE_duty;		// Wert zwischen 0 und 255 (0 = 0%, 255 = 100%)
	bool LEDBLUE_status;
	bool LEDWHITE_status;
	bool LEDRED_status;
	bool FAN_status;
} Module;

typedef enum{
	INIT,
	JOIN_NETWORK,
	START_REPORTING,
	NOTHING
} AppState_t;
#endif
// eof app.h
