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

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "lib/usb_srs_vendor_v1_2.h"

struct gpio_pin {
	unsigned char number;
	volatile unsigned char *ddr;
	volatile unsigned char *port;
};

int main(void)
{
	int i, j;
	uint8_t val;
	struct gpio_pin segments[] = {
		{1, &DDRD, &PORTD},
		{4, &DDRF, &PORTF},
		{5, &DDRF, &PORTF},
		{6, &DDRF, &PORTF},
		{7, &DDRF, &PORTF},
		{0, &DDRD, &PORTD},
		{4, &DDRD, &PORTD},
	};
	struct gpio_pin cathods[] = {
		{5, &DDRB, &PORTB},
		{6, &DDRB, &PORTB},
	};
	uint8_t segment_digits[] = { 63, 6, 91, 79, 102, 109, 125, 7, 127, 111, 119, 124, 57, 94, 121, 113 };

	/* Overwrite Fuse Bit */
	CLKPR = (1<<CLKPCE);  
	CLKPR = 0;
	/* Start USB IP */
	usb_init_device();

	/* Init Pins */
	for(i=0; i<7; i++) 
		*(segments[i].ddr) |= (1<<segments[i].number);
	for(i=0; i<2; i++)
		*(cathods[i].ddr) |= (1<<cathods[i].number);

	while (1) {
		for(j=0; j<2; j++) {
			val = (display >> (4*j)) & 0xf;
			*(cathods[0].port) |= (1<<cathods[0].number);
			*(cathods[1].port) |= (1<<cathods[1].number);
			*(cathods[j].port) &= ~(1<<cathods[j].number);

			for(i=0; i<7; i++) {
				*(segments[i].port) &= ~(1<<segments[i].number);
				*(segments[i].port) |= (((segment_digits[val] & (1<<i)) > 0) << segments[i].number);
			}
			_delay_ms(10);
		}
	}
}
