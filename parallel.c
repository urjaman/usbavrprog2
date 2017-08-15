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
#include "flash.h"
#include "parallel.h"
#include "uart.h"

static void parallel_chip_enable(void) {
	DDRF |= _BV(1);
	PORTF &= ~_BV(1);
}

void parallel_init(void) {
	DDRB = 0xFF;
	DDRC = 0xFF;
	PORTE = 0x8B;
	DDRE = 0x73;
	XMCRB = _BV(XMM0);
	XMCRA = _BV(SRE) | _BV(SRW11);
	parallel_chip_enable();
}

static void parallel_chip_disable(void) {
	PORTF |= _BV(1);
	DDRF &= ~_BV(1);
}

void parallel_safe(void) {
	parallel_chip_disable();
	XMCRA = 0;
	DDRC = 0;
	DDRB = 0;
	DDRE = 0;
}


static void* parallel_setaddr(uint32_t addr) {
	PORTC = ((uint16_t)addr >> 8) & 0xFF;
	PORTE = (PORTE & 0x8F) | (((addr >> 16) & 0x7) << 4);
	return (void*)((uint16_t)addr | 0x8000);
}

void parallel_readn(uint32_t addr, uint32_t len) {
	void *m = parallel_setaddr(addr);
	do {
		uint8_t txc = uart_send_getfree();
		if (txc > len) txc = len;
		len -= txc;
		do {
			uint8_t d;
			asm (
				"out %2, %A0"	"\n\t"
				"ld %1, %a0+"	"\n\t"
				: "=e" (m), "=r" (d) : "I" (_SFR_IO_ADDR(PORTB)), "0" (m)
			);
			uart_bulksend(d);
			if (!m) {
				addr = (addr & ~0x7FFF) + 0x8000;
				m = parallel_setaddr(addr);
			}
		} while (--txc);
	} while (len);
}

uint8_t parallel_read(uint32_t addr) {
	void *m = parallel_setaddr(addr);
	uint8_t d;
	asm (
		"out %2, %A0"	"\n\t"
		"ld %1, %a0+"	"\n\t"
		: "=e" (m), "=r" (d) : "I" (_SFR_IO_ADDR(PORTB)), "0" (m)
	);
	return d;
}

void parallel_write(uint32_t addr, uint8_t data) {
	void *m = parallel_setaddr(addr);
	asm volatile (
		"out %2, %A0"	"\n\t"
		"st %a0+, %1"	"\n\t"
		: "=e" (m) : "r" (data),  "I" (_SFR_IO_ADDR(PORTB)), "0" (m)
	);
}
