CFLAGS = -Os -DF_CPU=16000000UL -mmcu=atmega32u4 -Ilib 
SOURCES = lib/usb_srs_vendor_v1_2.c main.c
TARGET = usb_hello

all:
	avr-gcc -o ${TARGET}.elf ${SOURCES} ${CFLAGS}
	avr-objcopy -O ihex ${TARGET}.elf ${TARGET}.hex

clean:
	rm -rf *.hex *.elf
