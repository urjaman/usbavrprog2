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
#include "adc.h"

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

static void send_hdrval(PGM_P hdr, uint32_t val)
{
	sendstr_P(hdr);
	u32outdec(val);
}

static void send_pindv(PGM_P hdr, uint8_t mask)
{
	send_hdrval(hdr, (PIND & mask) ? 1 : 0 );
}

CIFACE_APP(miso_cmd, "MISO")
{
	send_pindv(PSTR("MISO:"), _BV(MISO));
}

CIFACE_APP(spistat_cmd, "SPISTAT")
{
	send_hdrval(PSTR("VSPI: "), measure_vspi() );
	send_pindv(PSTR("\r\nMISO: "), _BV(MISO));
	send_pindv(PSTR("\r\nWP: "), _BV(SPI_WP));
	send_pindv(PSTR("\r\nHOLD: "), _BV(SPI_HOLD));

	send_pindv(PSTR("\r\n!SPI_EN: "), _BV(SPI_EN));
	send_pindv(PSTR("\r\n!CS: "), _BV(SS));
	send_pindv(PSTR("\r\nSCK: "), _BV(SCK));
	send_pindv(PSTR("\r\nMOSI: "), _BV(MOSI));
}

static void spi_override(uint8_t set, uint8_t clear)
{
	spi_enable();
	spi_hw_off();
	SPI_PORT = (SPI_PORT & ~clear) | set;
	spistat_cmd();
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

CIFACE_APP(listen_cmd, "LISTEN")
{
	
}

CIFACE_APP(spioff_cmd, "SPIOFF")
{
	spi_disable();
}


CIFACE_APP(f_cpu_cmd, "FREQ")
{
	if (CLKPR) sendstr_P(PSTR("8 Mhz (CLKPR set)"));
	else sendstr_P(PSTR("16 Mhz (No div)"));
}
