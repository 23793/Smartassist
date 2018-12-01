#ifndef _APP_H
#define _APP_H

#define LEDWHITE	PE4
#define LEDRED		PE2
#define LEDBLUE		PE3
#define FAN			PE7

#define turnOff(PIN)	(PORTE &= ~(1<<PIN))
#define turnOn(PIN)		(PORTE |= (1<<PIN))
#define isOn(PIN)		(PINE & (1<<PIN))
#define isOff(PIN)		(!(PINE & (1<<PIN)))
#define toggle(PIN)		((PINE & (1<<PIN)) ? (PORTE &= ~(1<<PIN)) : (PORTE |= (1<<PIN)))

//Addresse TemperatureSensor
#define LM73_DEVICE_ADDRESS 0x4D

// Variablen f?r Temperaturmessung
#define APP_TEMPERATURE_MEASUREMENT_MEASURED_VALUE_ATTRIBUTE_VALUE       0x5000
#define APP_TEMPERATURE_MEASUREMENT_MIN_MEASURED_VALUE_ATTRIBUTE_VALUE   0x954d
#define APP_TEMPERATURE_MEASUREMENT_MAX_MEASURED_VALUE_ATTRIBUTE_VALUE   0x7fff
#define APP_TEMPERATURE_MEASUREMENT_TOLERANCE_ATTRIBUTE_VALUE            0x0100

#define APP_TEMPERATURE_MEASUREMENT_MEASURED_VALUE_PERIODIC_CHANGE       1000

typedef enum{
	INIT,
	START_NETWORK,
	REG_ENDPOINT,
	NOTHING
} AppState_t;
#endif
// eof app.h
