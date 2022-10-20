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

int main(void)
{
	/* Overwrite Fuse Bit */
	CLKPR = (1<<CLKPCE);  
	CLKPR = 0;
	/* Start USB IP */
	usb_init_device();
	/* Init GPIO for LED*/
	DDRB |= (1<<0);

	while (1) {
		PORTB ^= (1<<0);
		_delay_ms(500);
	}
}
