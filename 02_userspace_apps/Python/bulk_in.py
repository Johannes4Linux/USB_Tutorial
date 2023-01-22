import usb.core
import sys

dev = usb.core.find(idVendor = 0x03eb, idProduct=1)

if dev == None:
    print("No USB device found!")
    sys,exit(0)

# Get Configuration
cfg = dev.get_active_configuration()

# Get Interface
intf = cfg[(0,0)] # (index, alternate settings index)

# Get Endpoint
ep2 = intf[1]

data = ep2.read(8)

if data[1] = 1:
    print("Button pressed!")
else:
    print("Button not pressed!")

