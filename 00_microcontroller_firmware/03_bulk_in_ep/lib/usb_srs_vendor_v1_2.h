/* Copyright (c)   2022 Johannes 4Linux  johannes[at]gnu-linux[dot]rocks
                   2013 Guy WEILER       weigu[at]weigu[dot]lu

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.

   Licence: GPL-3        */

#ifndef _USB_SRS_H_
#define _USB_SRS_H_

#include <avr/io.h>
#include <stdint.h>
#include <stdbool.h>

/* Macros to make program easier to read */
#define CBI(adr,bit)    (adr &= ~(1<<bit)) /* Clear bit in SF-register (ASM) */
#define SBI(adr,bit)    (adr |=  (1<<bit)) /* Set bit in SF-register (ASM) */
#define low(x)   ((x) & 0xFF)
#define high(x)  (((x)>>8) & 0xFF)

/* Prescaler for USB PLL */
#define PLLPRE 0x10 /* Value for 16MHz crystal */

/*  ty TYPE:        0 = Control, 1 = Isochron, 2 = Bulk 3 = Interrupt
    di DIRECTION:   0 = OUT, 1 = IN (not for Control)
    si SIZE:        0 = 8 Bytes, 1 = 16 Bytes, 2 = 32 Bytes, 3 = 64 Bytes,
                    4 = 128 Bytes (only EP1), 5 = 256 Bytes (EP1), all others reserved
    ba BANK:        0 = 1 Bank, 1 = 2 Banks, all others reserved

Endpoint 0, Control OUT, 8 byte FIFO */
#define Ep0_ty 0    /* Type Control */
#define Ep0_di 0    /* Direction OUT */
#define Ep0_si 0    /* Size: 8 Bytes */
#define Ep0_ba 0    /* 1 Bank */
#define Ep0_fs 8    

/* Endpoint 1, BULK OUT, 8 byte FIFO */
#define Ep1_ty 2    /* Type BULK */
#define Ep1_di 0    /* Direction OUT */
#define Ep1_si 0    /* Size: 8 Bytes */
#define Ep1_ba 0    /* 1 Bank */
#define Ep1_fs 8

/* Endpoint 2, BULK OUT, 8 byte FIFO */
#define Ep2_ty 2    /* Type BULK */
#define Ep2_di 1    /* Direction IN */
#define Ep2_si 0    /* Size: 8 Bytes */
#define Ep2_ba 0    /* 1 Bank */
#define Ep2_fs 8

/* Number of Endpoints without EP0 */
#define Nr_eps 2
#define wTotalLength 9+9+(7*Nr_eps)

/* Status codes */
#define Lang_i     0   /* LanguageDescriptorIndex */
#define Manu_i     1   /* ManufacturerStringIndex */
#define Prod_i     2   /* ProductStringIndex */
#define Seri_i     3   /* SerialNumberStringIndex */
#define Intf_i     2   /* InterfaceStringIndex */

/* base functions */
void usb_init_device(void);
void usb_init_endpoint(uint8_t nu,uint8_t ty,uint8_t di,uint8_t si,uint8_t ba);

/* functions for  enumeration */
void usb_ep0_setup(void);
void usb_send_descriptor(uint8_t de[] ,uint8_t db);

/* Global variables for EP */
extern volatile uint8_t ep1_buf[8];
extern volatile uint8_t ep1_flag;
extern volatile uint8_t ep2_buf[8];
extern volatile uint8_t ep2_flag;

void usb_ep1_out(void);
void usb_ep2_in(void);

#endif
