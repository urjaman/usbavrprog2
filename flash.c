#include "main.h"
#include "flash.h"
#include "frser.h"
#include "spihw.h"
#include "uart.h"
#include "parallel.h"

static uint8_t flash_prot_in_use = 0;

void flash_select_protocol(uint8_t allowed_protocols)
{
	if (allowed_protocols & CHIP_BUSTYPE_SPI) spi_enable();
	else spi_disable();
	if ((flash_prot_in_use = allowed_protocols & CHIP_BUSTYPE_PARALLEL)) {
		parallel_init();
	} else {
		parallel_safe();
	}
}

void flash_set_safe(void)
{
	spi_disable();
	parallel_safe();
}

uint8_t flash_read(uint32_t addr)
{
	switch (flash_prot_in_use) {
		case 0:
		default:
			return 0xFF;
		case CHIP_BUSTYPE_PARALLEL:
			return parallel_read(addr);
	}
}

void flash_readn(uint32_t addr, uint32_t len)
{
	switch (flash_prot_in_use) {
		case 0:
		default:
			while (len--) SEND(0xFF);
			return;
		case CHIP_BUSTYPE_PARALLEL:
			parallel_readn(addr,len);
			return;
	}
}

void flash_write(uint32_t addr, uint8_t data)
{
	switch (flash_prot_in_use) {
		case 0:
		default:
			return;
		case CHIP_BUSTYPE_PARALLEL:
			parallel_write(addr,data);
			return;
	}
}
