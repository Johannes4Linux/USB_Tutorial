import usb.core
import sys

dev = usb.core.find(idVendor = 0x03eb, idProduct=1)

if dev == None:
    print("No USB device found!")
    sys.exit(0)

# Get Configuration
cfg = dev.get_active_configuration()

# Get Interface
intf = cfg[(0,0)] # (index, alternate settings index)

# Get Endpoint
ep = intf[0]

# Set up data
data = bytearray(8)
for i in range(4):
    data[i] = 0x7

# Write to BULK OUT EP
ep.write(data)


