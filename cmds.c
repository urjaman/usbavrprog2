/*
 * Copyright (C) 2017 Urja Rannikko <urjaman@gmail.com>
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
#include "uart.h"
#include "console.h"
#include "lib.h"
#include "ciface.h"
#include "appdb.h"
#include "spihw.h"

static void adc_init(void) {
	DIDR0 = _BV(ADC0D);
}

static uint16_t adc_sample(void) {
	ADCSRA = _BV(ADEN) | _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0) | _BV(ADSC);
	while (ADCSRA & _BV(ADSC));
	return ADC;
}

static void adc_off(void) {
	ADMUX = 0;
	ADCSRA = 0;
}

static uint16_t measure_vspi(void) {
	adc_init(); //fixme
	ADMUX = _BV(REFS1) | _BV(REFS0); // ADC0 w/ 2.56V ref
	/* Discard 2 samples during init. */
	adc_sample();
	adc_sample();
	uint16_t r = 0;
	for (uint8_t i=0; i<5*4; i++) r += adc_sample();
	adc_off();
	/* Result is in mV (3300 = 3.3V) */
	return r/4;
}

CIFACE_APP(vspi_cmd, "VSPI")
{
	luint2outdual(measure_vspi());
}

