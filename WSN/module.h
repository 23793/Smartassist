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

typedef enum{
	INIT,
	START_NETWORK,
	REG_ENDPOINT,
	NOTHING
} AppState_t;
#endif
// eof app.h