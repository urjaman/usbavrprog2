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

static uint16_t adc_bigsample(uint8_t mux, uint8_t ss) {
	adc_init(); //fixme
	ADMUX = mux; // ADC0 w/ 2.56V ref
	/* Discard 2 samples during init. */
	adc_sample();
	adc_sample();
	uint16_t r = 0;
	for (uint8_t i=0; i<ss; i++) r += adc_sample();
	return r;
}

static uint16_t measure_vspi(void) {
	// ADC0 w/ 2.56V ref
	uint16_t r = adc_bigsample(_BV(REFS1) | _BV(REFS0), 5*4);
	adc_off();
	/* Result is in mV (3300 = 3.3V) */
	return (r+2)/4;
}

static uint16_t measure_vcc(void) {
	// 1.1V BG w/ AVCC ref
	uint16_t r = (adc_bigsample(_BV(REFS0) | 0b11110, 4*4)+2)/4;
	adc_off();
	/* This measurement fruks up VSPI measurements, so flush a little. */
	for (uint8_t i=0;i<5;i++) measure_vspi();
	// 1.1 * 1024 * 1000 * 4
	return (4505600UL+(r/2)) / r;
}

CIFACE_APP(vspi_cmd, "VSPI")
{
	luint2outdual(measure_vspi());
}

CIFACE_APP(vcc_cmd, "VCC")
{
	luint2outdual(measure_vcc());
}

CIFACE_APP(spitest_cmd, "SPILOOP")
{
	spi_enable();
	spi_hw_on();
	luint2outdual(spi_txrx(0xA5));
	luint2outdual(spi_txrx(0x5A));
	luint2outdual(spi_txrx(0xDB));
}

static void spi_override(uint8_t set, uint8_t clear)
{
	spi_enable();
	spi_hw_off();
	SPI_PORT = (SPI_PORT & ~clear) | set;
}

CIFACE_APP(cs0_cmd, "CS0")
{
	spi_override(0, _BV(SS));
}

CIFACE_APP(cs1_cmd, "CS1")
{
	spi_override( _BV(SS), 0);
}

CIFACE_APP(sck0_cmd, "SCK0")
{
	spi_override(0, _BV(SCK));
}

CIFACE_APP(sck1_cmd, "SCK1")
{
	spi_override(_BV(SCK), 0);
}


CIFACE_APP(mosi0_cmd, "MOSI0")
{
	spi_override(0, _BV(MOSI));
}

CIFACE_APP(mosi1_cmd, "MOSI1")
{
	spi_override(_BV(MOSI), 0);
}

CIFACE_APP(miso_cmd, "MISO")
{
	sendstr_P( (PIND & _BV(MISO)) ? PSTR("1") : PSTR("0") );
}

