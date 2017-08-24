#define F_CPU 16000000UL
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <avr/power.h>
#include <avr/sleep.h>
#include <util/delay.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <alloca.h>
#include <inttypes.h>
#include <setjmp.h>
#include "ciface.h"

#define LED1(x) do { if (x) PORTF |= _BV(2); else PORTF &= ~_BV(2); } while(0)
#define LED2(x) do { if (x) PORTF |= _BV(3); else PORTF &= ~_BV(3); } while(0)
