/* Based on arduino-usbserial and LUFA. but made
 * into a stand-alone LUFA subset and heavily modified
 * by Urja Rannikko 2015. My modifications are under the LUFA
 * License below. */

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

/** \file
 *
 *  Main source file for the fast-usbserial project. This file contains the main tasks of
 *  the project and is responsible for the initial application hardware configuration.
 */

#include "main.h"
#include "fast-usbserial.h"
/* We implement the old uart.h API for libfrser */
#include "uart.h"

/** LUFA CDC Class driver interface configuration and state information. This structure is
 *  passed to all CDC Class driver functions, so that multiple instances of the same class
 *  within a device can be differentiated from one another.
 */
USB_ClassInfo_CDC_Device_t VirtualSerial_CDC_Interface;

static uint8_t usb_rxpacket_leftb = 0;
static uint8_t usb_txpacket_leftb = 0;

static void usb_process(void) {
	Endpoint_SelectEndpoint(ENDPOINT_CONTROLEP);
	do {
		if (Endpoint_IsSETUPReceived())
		  USB_Device_ProcessControlRequest();
	} while (USB_DeviceState != DEVICE_STATE_Configured);
}


uint8_t uart_recv(void) {
	do {
		if (usb_rxpacket_leftb) {
			uint8_t d;
			Endpoint_SelectEndpoint(CDC_RX_EPNUM);
			d = Endpoint_Read_Byte();
			usb_rxpacket_leftb--;
			if (!usb_rxpacket_leftb)
				Endpoint_ClearOUT();
			return d;
		}
		usb_process();
		usb_rxpacket_leftb = CDC_Device_BytesReceived(&VirtualSerial_CDC_Interface);
		if ((!usb_rxpacket_leftb)&&(usb_txpacket_leftb)) {
			Endpoint_SelectEndpoint(CDC_TX_EPNUM);
	                Endpoint_ClearIN(); /* Go data, GO. */
	                usb_txpacket_leftb = 0;
	        }
	} while (1);
}

void uart_send(uint8_t d) {
	do {
		if (usb_txpacket_leftb) {
			Endpoint_SelectEndpoint(CDC_TX_EPNUM);
		        Endpoint_Write_Byte(d);
		        usb_txpacket_leftb--;
		        if (usb_txpacket_leftb == 1) {
		                Endpoint_ClearIN(); /* Go data, GO. */
		        	usb_txpacket_leftb = 0;
		        }
		        return;
		}
		usb_process();
		if (CDC_Device_SendByte_Prep(&VirtualSerial_CDC_Interface) == 0)
			usb_txpacket_leftb = CDC_IN_EPSIZE;
	} while (1);
}

uint8_t uart_isdata(void) {
	if (usb_rxpacket_leftb) {
		Endpoint_SelectEndpoint(CDC_RX_EPNUM);
		return usb_rxpacket_leftb;
	}
	usb_process();
	usb_rxpacket_leftb = CDC_Device_BytesReceived(&VirtualSerial_CDC_Interface);
	return usb_rxpacket_leftb;
}

uint8_t uart_send_getfree(void) {
	if (usb_txpacket_leftb) {
		Endpoint_SelectEndpoint(CDC_TX_EPNUM);
		return usb_txpacket_leftb-1;
	}
	do {
		usb_process();
		if (CDC_Device_SendByte_Prep(&VirtualSerial_CDC_Interface) == 0) {
			usb_txpacket_leftb = CDC_IN_EPSIZE;
			return usb_txpacket_leftb-1;
		}
	} while (1);
}

void uart_wait_txdone(void) {
	if (usb_txpacket_leftb) {
		Endpoint_SelectEndpoint(CDC_TX_EPNUM);
                Endpoint_ClearIN(); /* Go data, GO. */
                usb_txpacket_leftb = 0;
        }
}

/* These are to be used very responsibly by code that knows what its doing. */
void uart_recv_ctrl_cnt(uint8_t b) {
	usb_rxpacket_leftb -= b;
	if (!usb_rxpacket_leftb)
		Endpoint_ClearOUT();
}

void uart_send_ctrl_cnt(uint8_t b) {
        usb_txpacket_leftb -= b;
        if (usb_txpacket_leftb == 1) {
                Endpoint_ClearIN(); /* Go data, GO. */
        	usb_txpacket_leftb = 0;
        }
}


/** Configures the board hardware and chip peripherals for the demo's functionality. */
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
