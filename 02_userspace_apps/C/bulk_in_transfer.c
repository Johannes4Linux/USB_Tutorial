#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <libusb-1.0/libusb.h>

int main(int argc, char **argv) {
	int status, value;
	libusb_device_handle *dev = NULL;
	char buffer[8];
	int transferred, i;

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

	memset(buffer, 0, 8);
	status = libusb_bulk_transfer(dev, 0x80 | 2, buffer, 8, &transferred, 100);
	printf("Status: %d, Bytes transferred: %d\n", status, transferred);

	printf("Button is %spressed\n", (buffer[1] == 1) ? "" : "not ");

	libusb_close(dev);
	libusb_exit(NULL);
	return 0;
}
