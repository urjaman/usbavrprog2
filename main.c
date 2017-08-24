/*
 * Copyright (C) 2010,2017 Urja Rannikko <urjaman@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include "main.h"
#include "fast-usbserial.h"
#include "uart.h"
#include "frser.h"
#include "spihw.h"
#include "adc.h"

int main(void) {
	CLKPR = _BV(CLKPCE);
	CLKPR = 0; // TODO: 3.3V support
	adc_init();
	SetupHardware();
	spi_init();
	sei();
	frser_main();
}


