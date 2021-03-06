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
#include "adc.h"

void adc_init(void)
{
	DIDR0 = _BV(ADC0D);
}

static uint16_t adc_sample(void)
{
	/* Deal with being run at 2Mhz, 8Mhz or 16Mhz (2 Mhz before we have info on VCC) */
	uint8_t ps = _BV(ADPS2);
	uint8_t pr = CLKPR;
	if (pr == 1) ps |= _BV(ADPS1);
	if (!pr) ps |= _BV(ADPS1) | _BV(ADPS0);
	ADCSRA = _BV(ADEN) | ps | _BV(ADSC);
	while (ADCSRA & _BV(ADSC));
	return ADC;
}

static void adc_off(void)
{
	ADMUX = 0;
	ADCSRA = 0;
}

static uint16_t adc_bigsample(uint8_t mux, uint8_t ss)
{
	ADMUX = mux; // ADC0 w/ 2.56V ref
	/* Discard 2 samples during init. */
	adc_sample();
	adc_sample();
	uint16_t r = 0;
	for (uint8_t i=0; i<ss; i++) r += adc_sample();
	return r;
}

uint16_t measure_vspi(void)
{
	// ADC0 w/ 2.56V ref
	uint16_t r = adc_bigsample(_BV(REFS1) | _BV(REFS0), 5*4);
	adc_off();
	/* Result is in mV (3300 = 3.3V) */
	return (r+2)/4;
}

uint16_t measure_vcc(void)
{
	// 1.1V BG w/ AVCC ref
	uint16_t r = (adc_bigsample(_BV(REFS0) | 0b11110, 4*4)+2)/4;
	adc_off();
	/* This measurement fruks up VSPI measurements, so flush a "little". */
	for (uint8_t i=0; i<5; i++) measure_vspi();
	// 1.1 * 1024 * 1000 * 4
	return (4505600UL+(r/2)) / r;
}

