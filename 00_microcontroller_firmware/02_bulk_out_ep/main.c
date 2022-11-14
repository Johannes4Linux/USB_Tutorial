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

int main(void) {
	int i;
	struct gpio_pin pins[] = {
		{5, &DDRB, &PORTB},
		{4, &DDRB, &PORTB},
		{6, &DDRE, &PORTE},
		{7, &DDRD, &PORTD},
		{6, &DDRC, &PORTC},
		{4, &DDRD, &PORTD},
		{0, &DDRD, &PORTD},
		{1, &DDRD, &PORTD},
	};

	/* Init Pins */
	for(i=0; i<8; i++)
		*(pins[i].ddr) |= (1<<pins[i].number);

	/* Overwrite Fuse Bit */
	CLKPR = (1<<CLKPCE);  
	CLKPR = 0;
	/* Start USB IP */
	usb_init_device();
	/* Init GPIO for LED*/
	DDRB |= (1<<0);

	while (1) {
		if(ep1_flag == 1){
			cli();
			for(i=0; i<8; i++) {
				if(ep1_buf[i] & 0x1) {
					*(pins[i].port) &= ~(1<<pins[i].number);
					if(ep1_buf[i] & 0x2)
						*(pins[i].port) |= (1<<pins[i].number);
				}
			}
			ep1_flag = 0;
			sei();
		}
	}
}
