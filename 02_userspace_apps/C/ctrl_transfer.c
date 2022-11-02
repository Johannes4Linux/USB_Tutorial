#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <libusb-1.0/libusb.h>

int main(int argc, char **argv) {
	int status, value;
	libusb_device_handle *dev = NULL;

	status = libusb_init(NULL);
	if(status != 0) {
		perror("Error init libusb");
		return -status;
	}

	dev = libusb_open_device_with_vid_pid(NULL, 0x03eb, 0x0001);
	if(dev == NULL) {
		printf("Error! Could not find USB device!\n");
		libusb_exit(NULL);
		return -1;
	}

	if(argc > 1) {
		value = (int) strtol(argv[1], NULL, 0);
		libusb_control_transfer(dev, 0x40, 0x1, value, 0, NULL, 0, 100);
	}

	value = 0;
	status = libusb_control_transfer(dev, 0xC0, 0x2, 0, 0, (unsigned char *) &value, 1, 100);
	if(status != 1) {
		printf("Error! Could not read from USB device!\n");
		libusb_close(dev);
		libusb_exit(NULL);
		return -1;
	}
	printf("Current displayed value: 0x%x\n", value);


	libusb_close(dev);
	libusb_exit(NULL);
	return 0;
}
