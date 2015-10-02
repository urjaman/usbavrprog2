/*
 * This file is part of the frser-duino project.
 *
 * Copyright (C) 2015 Urja Rannikko <urjaman@gmail.com>
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
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include "main.h"
#include "uart.h"
#include "fast-usbserial.h"
#include "spihw.h"

/* This is just a bit of glue between parts of libfrser. */

void flash_set_safe(void) {
	spi_uninit();
	DDR_SPI &= ~_BV(MOSI);
	DDR_SPI &= ~_BV(SCK);
	DDR_SPI &= ~_BV(SS);
}

void flash_select_protocol(uint8_t allowed_protocols) {
	(void)allowed_protocols;
	SPI_PORT |= _BV(SS);
	SPI_PORT &= ~_BV(MOSI);
	SPI_PORT &= ~_BV(SCK);
	DDR_SPI = (1<<MOSI)|(1<<SCK)|(1<<SS);
	spi_init();
}

static void spi_spiop_start(uint32_t sbytes) {
	spi_select();
	if (sbytes) do {
		uint8_t rxc;
		do {
			rxc = uart_isdata();
		} while (!rxc);
		if (rxc > sbytes) rxc = sbytes;
		uint8_t dc = rxc;
		do {
			spi_txrx(Endpoint_Read_Byte());
		} while(--rxc);
		uart_recv_ctrl_cnt(dc);
		sbytes -= dc;
	} while (sbytes);
}

static void spi_spiop_end(uint32_t rbytes) {
	if (rbytes) do {
		SPDR = 0xFF;
		uint8_t txc = uart_send_getfree();
		if (txc > rbytes) txc = rbytes;
		uint8_t dc = txc;
		txc--;
		if (txc) do {
			loop_until_bit_is_set(SPSR,SPIF);
			Endpoint_Write_Byte(SPDR);
			SPDR = 0xFF;
		} while (--txc);
		rbytes -= dc;
		loop_until_bit_is_set(SPSR,SPIF);
		Endpoint_Write_Byte(SPDR);
		uart_send_ctrl_cnt(dc);
	} while (rbytes);
	spi_deselect();
}

void flash_spiop(uint32_t sbytes, uint32_t rbytes) {
	spi_spiop_start(sbytes);
	SEND(S_ACK);
	spi_spiop_end(rbytes);
}
