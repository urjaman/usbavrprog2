/*
 * Copyright (C) 2010,2011.2017 Urja Rannikko <urjaman@gmail.com>
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

uint8_t uart_recv(void);
void uart_send(uint8_t val);
void uart_wait_txdone(void);

/* These can be used to go behind the scenes with the USB "uart". */
uint8_t uart_isdata(void);
uint8_t uart_bulkrecv();

uint8_t uart_send_getfree(void);
uint8_t uart_send_getfree_noblock(void);
void uart_bulksend(uint8_t d);


#define RECEIVE() uart_recv()
#define SEND(n) uart_send(n)
#define UART_BUFLEN 1024
#define UART_POLLED_TX
#define UARTTX_BUFLEN 0
/* Fake, roughly USB 1.1 bandwidth. */
#define BAUD 8000000
