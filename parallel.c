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

static void parallel_chip_enable(void)
{
	DDRF |= _BV(1);
	PORTF &= ~_BV(1);
}

void parallel_init(void)
{
	DDRB = 0xFF;
	DDRC = 0xFF;
	PORTE = 0x8B;
	DDRE = 0x73;
	XMCRB = _BV(XMM0);
	XMCRA = _BV(SRE) | _BV(SRW11);
	parallel_chip_enable();
}

static void parallel_chip_disable(void)
{
	PORTF |= _BV(1);
	DDRF &= ~_BV(1);
}

void parallel_safe(void)
{
	parallel_chip_disable();
	XMCRA = 0;
	DDRC = 0;
	DDRB = 0;
	DDRE = 0;
}

static void* parallel_setaddr(uint32_t addr)
{
	PORTC = ((uint16_t)addr >> 8) & 0xFF;
	PORTE = (PORTE & 0x8F) | (((addr >> 16) & 0x7) << 4);
	return (void*)((uint16_t)addr | 0x8000);
}

void parallel_readn(uint32_t addr, uint32_t len)
{
	do {
		volatile uint8_t *m = parallel_setaddr(addr);
		uint16_t chunkl = 0x8000 - (addr & 0x7FFF);
		if (chunkl > len) chunkl = len;
		len -= chunkl;
		addr += chunkl;
		do {
			uint8_t txc = uart_send_getfree();
			if (txc > chunkl) txc = chunkl;
			chunkl -= txc;
			do {
				PORTB = ((uint16_t)m) & 0xFF;
				uart_bulksend(*m++);
			} while (--txc);
		} while (chunkl);
	} while (len);
}

uint8_t parallel_read(uint32_t addr)
{
	volatile uint8_t *m = parallel_setaddr(addr);
	PORTB = ((uint16_t)m) & 0xFF;
	return *m;
}

void parallel_write(uint32_t addr, uint8_t data)
{
	volatile uint8_t *m = parallel_setaddr(addr);
	PORTB = ((uint16_t)m) & 0xFF;
	*m = data;
}
