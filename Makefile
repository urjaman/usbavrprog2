##
## Copyright (C) 2010,2011,2015 Urja Rannikko <urjaman@gmail.com>
##
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 2 of the License, or
## (at your option) any later version.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##

PROJECT=usbavrprog2
DEPS=uart.h main.h flash.h parallel.h Makefile

SOURCES=main.c flash.c spihw.c parallel.c Descriptors.c fast-usbserial.c USB-Drivers/ConfigDescriptor.c USB-Drivers/DeviceStandardReq.c USB-Drivers/Events.c USB-Drivers/USBController.c USB-Drivers/USBTask.c USB-Drivers/Device.c USB-Drivers/Endpoint.c USB-Drivers/SimpleCDC.c USB-Drivers/USBInterrupt.c

CC=avr-gcc
LD=avr-ld
OBJCOPY=avr-objcopy
MMCU=at90usb1287

AVRDUDECMD=avrdude -c avrispmkii -p m16u2 -P usb

#AVRBINDIR=/usr/avr/bin/

CFLAGS=-mmcu=$(MMCU) -Os -Wl,--relax -fno-inline-small-functions -fno-tree-switch-conversion -frename-registers -g -Wall -W -pipe -flto -fwhole-program -std=gnu99 $(DFLAGS)

LUFA_OPTS  = -D USB_DEVICE_ONLY
LUFA_OPTS += -D FIXED_CONTROL_ENDPOINT_SIZE=8
LUFA_OPTS += -D FIXED_NUM_CONFIGURATIONS=1
LUFA_OPTS += -D USE_FLASH_DESCRIPTORS
LUFA_OPTS += -D NO_DEVICE_SELF_POWER
LUFA_OPTS += -D NO_DEVICE_REMOTE_WAKEUP
LUFA_OPTS += -D DEVICE_STATE_AS_GPIOR=2
LUFA_OPTS += -D USE_STATIC_OPTIONS="(USB_DEVICE_OPT_FULLSPEED | USB_OPT_REG_ENABLED | USB_OPT_AUTO_PLL)"

CFLAGS += $(LUFA_OPTS)

include libfrser/Makefile.frser

all: $(PROJECT).out

$(PROJECT).hex: $(PROJECT).out
	$(AVRBINDIR)$(OBJCOPY) -j .text -j .data -O ihex $(PROJECT).out $(PROJECT).hex

$(PROJECT).bin: $(PROJECT).out
	$(AVRBINDIR)$(OBJCOPY) -j .text -j .data -O binary $(PROJECT).out $(PROJECT).bin

$(PROJECT).out: $(SOURCES) $(DEPS)
	$(AVRBINDIR)$(CC) $(CFLAGS) -I. -IUSB-Drivers -o $(PROJECT).out $(SOURCES)
	$(AVRBINDIR)avr-size $(PROJECT).out

asm: $(SOURCES) $(DEPS)
	$(AVRBINDIR)$(CC) $(CFLAGS) -S  -I. -o $(PROJECT).s $(SOURCES)


program: $(PROJECT).hex
	$(AVRBINDIR)$(AVRDUDECMD) -U flash:w:$(PROJECT).hex


clean:
	rm -f $(PROJECT).bin
	rm -f $(PROJECT).out
	rm -f $(PROJECT).hex
	rm -f $(PROJECT).s
	rm -f *.o


objdump: $(PROJECT).out
	$(AVRBINDIR)avr-objdump -xdC $(PROJECT).out | less
