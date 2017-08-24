/* Based on arduino-usbserial and LUFA. but made
 * into a stand-alone LUFA subset and heavily modified
 * by Urja Rannikko 2015,2017. My modifications are under the
 * LUFA License below. */

/*
             LUFA Library
     Copyright (C) Dean Camera, 2010.

  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com
*/

/*
  Copyright 2010  Dean Camera (dean [at] fourwalledcubicle [dot] com)

  Permission to use, copy, modify, distribute, and sell this
  software and its documentation for any purpose is hereby granted
  without fee, provided that the above copyright notice appear in
  all copies and that both that the copyright notice and this
  permission notice and warranty disclaimer appear in supporting
  documentation, and that the name of the author not be used in
  advertising or publicity pertaining to distribution of the
  software without specific, written prior permission.

  The author disclaim all warranties with regard to this
  software, including all implied warranties of merchantability
  and fitness.  In no event shall the author be liable for any
  special, indirect or consequential damages or any damages
  whatsoever resulting from loss of use, data or profits, whether
  in an action of contract, negligence or other tortious action,
  arising out of or in connection with the use or performance of
  this software.
*/

#include "main.h"
#include "fast-usbserial.h"
/* We implement the old uart.h API for libfrser */
#include "uart.h"

USB_ClassInfo_CDC_Device_t VirtualSerial_CDC_Interface;

static void usb_process(void)
{
	LED1(1);
	Endpoint_SelectEndpoint(ENDPOINT_CONTROLEP);
	do {
		if (Endpoint_IsSETUPReceived())
			USB_Device_ProcessControlRequest();
	} while (USB_DeviceState != DEVICE_STATE_Configured);
	LED1(0);
}

uint8_t uart_recv(void)
{
	do {
		Endpoint_SelectEndpoint(CDC_RX_EPNUM);
		do {
			uint8_t s = UEINTX;
			if (s & _BV(RWAL)) {
				return Endpoint_Read_Byte();
			}
			if (s & _BV(RXOUTI)) {
				UEINTX = s & ~((1 << RXOUTI) | (1 << FIFOCON));
				continue;
			}
		} while(0);
		usb_process();
		Endpoint_SelectEndpoint(CDC_TX_EPNUM);
		if (Endpoint_BytesInEndpoint()) {
			Endpoint_ClearIN(); /* Flush TX.. */
		}
	} while (1);
}

void uart_send(uint8_t d)
{
	do {
		Endpoint_SelectEndpoint(CDC_TX_EPNUM);
		uint8_t s = UEINTX;
		if (s & _BV(RWAL)) {
			Endpoint_Write_Byte(d);
			return;
		}
		/* Flush */
		UEINTX = s & ~((1 << TXINI) | (1 << FIFOCON));
		usb_process();
	} while (1);
}

uint8_t uart_isdata(void)
{
	Endpoint_SelectEndpoint(CDC_RX_EPNUM);
	uint8_t s = UEINTX;
	if (s & _BV(RXOUTI)) {
		uint8_t b;
		if ((b = Endpoint_BytesInEndpoint())) return b;
		UEINTX = s & ~((1 << RXOUTI) | (1 << FIFOCON));
	}
	usb_process();
	return 0;
}

uint8_t uart_bulkrecv(void)
{
	return Endpoint_Read_Byte();
}


uint8_t uart_send_getfree_noblock(void)
{
	Endpoint_SelectEndpoint(CDC_TX_EPNUM);
	uint8_t s = UEINTX;
	if (s & _BV(RWAL)) {
		return CDC_IN_EPSIZE - Endpoint_BytesInEndpoint();
	}
	/* Flush */
	UEINTX = s & ~((1 << TXINI) | (1 << FIFOCON));

	/* Look once again. */
	s = UEINTX;
	if (s & _BV(RWAL)) {
		return CDC_IN_EPSIZE;
	}
	return 0;
}

uint8_t uart_send_getfree(void)
{
	do {
		uint8_t r = uart_send_getfree_noblock();
		if (r) return r;
		usb_process();
	} while(1);
}



void uart_bulksend(uint8_t d)
{
	Endpoint_Write_Byte(d);
}

void uart_wait_txdone(void)
{
	Endpoint_SelectEndpoint(CDC_TX_EPNUM);
	if (Endpoint_BytesInEndpoint()) {
		Endpoint_ClearIN(); /* Flush TX.. */
	}
}

void SetupHardware(void)
{
	/* Disable watchdog if enabled by bootloader/fuses */
	MCUSR &= ~(1 << WDRF);
	wdt_disable();

	/* Hardware Initialization */
	USB_Init();

}

/** Event handler for the library USB Configuration Changed event. */
void EVENT_USB_Device_ConfigurationChanged(void)
{
	CDC_Device_ConfigureEndpoints(&VirtualSerial_CDC_Interface);
}

/** Event handler for the library USB Unhandled Control Request event. */
void EVENT_USB_Device_UnhandledControlRequest(void)
{
	CDC_Device_ProcessControlRequest(&VirtualSerial_CDC_Interface);
}

/** Event handler for the CDC Class driver Line Encoding Changed event.
 *
 *  \param[in] CDCInterfaceInfo  Pointer to the CDC class interface configuration structure being referenced
 */
void EVENT_CDC_Device_LineEncodingChanged(USB_ClassInfo_CDC_Device_t* const CDCInterfaceInfo)
{

}

/** Event handler for the CDC Class driver Host-to-Device Line Encoding Changed event.
 *
 *  \param[in] CDCInterfaceInfo  Pointer to the CDC class interface configuration structure being referenced
 */
void EVENT_CDC_Device_ControLineStateChanged(USB_ClassInfo_CDC_Device_t* const CDCInterfaceInfo)
{

}
