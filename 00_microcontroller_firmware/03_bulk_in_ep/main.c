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
	volatile unsigned char *pin;
	unsigned char old_val;
};

int main(void) {
	int i, counter = 0;;
	unsigned char new_val;
	struct gpio_pin pins[] = {
		{5, &DDRB, &PORTB, &PINB, 2},
		{4, &DDRB, &PORTB, &PINB, 2},
		{6, &DDRE, &PORTE, &PINE, 2},
		{7, &DDRD, &PORTD, &PIND, 2},
		{6, &DDRC, &PORTC, &PINC, 2},
		{4, &DDRD, &PORTD, &PIND, 2},
		{0, &DDRD, &PORTD, &PIND, 2},
		{1, &DDRD, &PORTD, &PIND, 2},
	};

	/* Initially all pins are configured as inputs */

	/* Overwrite Fuse Bit */
	CLKPR = (1<<CLKPCE);  
	CLKPR = 0;
	/* Start USB IP */
	usb_init_device();
	/* Init GPIO for LED*/
	DDRB |= (1<<6);

	while (1) {
		/* Check if a new value is to set for the ODR */
		if(ep1_flag == 1){
			cli();
			for(i=0; i<8; i++) {
				/* New ODR value */
				if(ep1_buf[i] & 0x1) {
					*(pins[i].port) &= ~(1<<pins[i].number);
					if(ep1_buf[i] & 0x2)
						*(pins[i].port) |= (1<<pins[i].number);
				}
				/* Change Pin direction */
				if(ep1_buf[i] & 0x4) {
					*(pins[i].ddr) &= ~(1<<pins[i].number);
					if(ep1_buf[i] & 0x8)
						*(pins[i].ddr) |= (1<<pins[i].number);
				}
			}
			ep1_flag = 0;
			sei();
		}
		/* Read  input pin */
		if(counter == 100) {
			cli();
			for(i=0; i<8; i++) 
				ep2_buf[i] = (*(pins[i].pin) & (1<<pins[i].number)) > 0;
			ep2_flag = 1;
			counter = 0;
			sei();
		}
		counter++;
		_delay_us(100);
	}
}
