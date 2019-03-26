#include "main.h"

void udelay(uint32_t usecs) {
	if (CLKPR) usecs = (usecs+1) / 2;
	if (usecs < 16) {
		uint8_t i=usecs;
		do { _delay_us(1); } while(--i);
		return;
	}
	usecs >>= 4; // div 16;
	do { _delay_us(16); } while(--usecs);
	return;
}
