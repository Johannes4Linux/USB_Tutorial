# A simple program to show how to use USB control transfers with pyusb
# Run program as root

import usb.core
import sys

dev = usb.core.find(idVendor=0x03eb, idProduct=0x1)
if(dev == None):
    print("Could not find device")
    sys.exit(255)

# Write to the device
dev.ctrl_transfer(0x40,    # bmRequestType
                  0x1,     # bRequest
                  0x12,    # wValue
                  0x0,     # wIndex
                  0x0      # Length of data or data
                  )

# Read from device
dev.ctrl_transfer(0xc0,    # bmRequestType
                  0x2,     # bRequest
                  0x0,     # wValue
                  0x0,     # wIndex
                  0x1      # Length of data or data
                  )


