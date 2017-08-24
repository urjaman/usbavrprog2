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

