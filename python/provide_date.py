#! /usr/bin/env python3
from datetime import datetime
import struct

import serial

now = datetime.now()

ms = ((now.hour * 60 + now.minute) * 60 + now.second) * 1000 + (now.microsecond // 1000)

print(ms)
print(hex(ms))

with serial.Serial('/dev/ttyUSB0', 9600) as port:
    port.write(b't' + struct.pack('>L', ms))
