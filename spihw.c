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
 *
 */

#include "main.h"
#include "frser.h"
#include "uart.h"
#include "fast-usbserial.h"
#include "spihw.h"
#include "flash.h"

#ifdef FRSER_FEAT_SPISPEED
uint32_t spi_set_speed(uint32_t hz)
{
	uint8_t spi_set_ubrr = 0;
	uint32_t f_cpu = F_CPU;
	if (CLKPR) f_cpu = f_cpu / 2;
	/* Range check. */
	if (hz>(f_cpu/2)) {
		spi_set_ubrr = 0;
	} else if (hz<(f_cpu/512)) {
		spi_set_ubrr = 255;
	} else {
		uint32_t bdiv = hz*2;
		uint32_t ubrr_vp = (f_cpu / bdiv)-1;
		// If the division is not perfect, increase the result (round down).
		if (f_cpu%bdiv) ubrr_vp++;
		spi_set_ubrr = ubrr_vp;
	}
	UBRR1 = spi_set_ubrr;
	uint32_t new_hz = f_cpu / (((int)spi_set_ubrr+1)*2);
	return new_hz;
}
#endif

/* These control the external drivers. */
void spi_disable(void)
{
	DDR_SPI &= ~_BV(SPI_XBUF);
	DDR_SPI &= ~_BV(SPI_EN);
}

void spi_enable(void)
{
	DDR_SPI |= _BV(SPI_EN);
	DDR_SPI |= _BV(SPI_XBUF);
}

/* For init and testing. */
void spi_hw_on(void)
{
	UCSR1C = _BV(UMSEL11)|_BV(UMSEL10);
	UBRR1 = CLKPR ? 1 : 3; /* Default to 2Mhz */
	UCSR1B = _BV(TXEN1)|_BV(RXEN1);
}

/* This initialises the SPI / UART MSPIM etc on boot. */
void spi_init(void)
{
	SPI_PORT = _BV(SS) | _BV(MISO) | _BV(SPI_WP) | _BV(SPI_HOLD);
	DDR_SPI = _BV(MOSI) | _BV(SCK) | _BV(SS);
	spi_hw_on();
}

void flash_spiop(uint32_t sbytes, uint32_t rbytes)
{
	spi_select();
	_delay_us(1);
	if (sbytes) {
		UCSR1B = _BV(TXEN1);
		do {
			uint8_t rxc;
			do {
				rxc = uart_isdata();
			} while (!rxc);
			if (rxc > sbytes) rxc = sbytes;
			sbytes -= rxc;
			do {
				UCSR1A = _BV(TXC1);
				UDR1 = uart_bulkrecv();
				loop_until_bit_is_set(UCSR1A, UDRE1);
			} while(--rxc);
		} while (sbytes);
		loop_until_bit_is_set(UCSR1A, TXC1);
	}
	SEND(S_ACK);
	if (rbytes) {
		UCSR1B = _BV(TXEN1)|_BV(RXEN1);
		do {
			UDR1 = 0xFF;
			uint8_t txc = uart_send_getfree();
			if (txc > rbytes) txc = rbytes;
			rbytes -= txc;
			txc--;
			uint8_t pxc = txc;
			while (txc) {
				uint8_t s = UCSR1A;
				if (s & _BV(RXC1)) {
					uint8_t d = UDR1;
					uart_bulksend(d);
					txc--;
				}
				if (s & _BV(UDRE1)) {
					if (pxc) {
						UDR1 = 0xFF;
						pxc--;
					}
				}
			}
			loop_until_bit_is_set(UCSR1A, RXC1);
			uart_bulksend(UDR1);
		} while (rbytes);
	}
	_delay_us(1);
	spi_deselect();
	_delay_us(1);
}

/* These are for testing cmd purposes. */
void spi_hw_off(void)
{
	UCSR1B = 0;
	UCSR1C = 0;
}

uint8_t spi_txrx(uint8_t d)
{
	UCSR1B = _BV(TXEN1)|_BV(RXEN1);
	UDR1 = d;
	loop_until_bit_is_set(UCSR1A, RXC1);
	return UDR1;
}
