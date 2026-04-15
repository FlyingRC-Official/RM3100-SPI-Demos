from machine import Pin, SPI
import time

from rm3100 import RM3100


# Replace these placeholders with the SPI bus and pins for your board.
spi = SPI(1, baudrate=1000000, polarity=0, phase=0, bits=8, firstbit=SPI.MSB)
cs = Pin(5, Pin.OUT, value=1)

sensor = RM3100(spi, cs, cycle_count=200)
sensor.begin()

print("RM3100 REVID: 0x{:02X}".format(sensor.read_revision()))
print("Reading RM3100 data over SPI...")

while True:
    raw_x, raw_y, raw_z = sensor.read_raw_xyz()
    ut_x, ut_y, ut_z = sensor.convert_raw_to_ut((raw_x, raw_y, raw_z))

    print("raw x={} y={} z={}".format(raw_x, raw_y, raw_z))
    print("uT  x={:.3f} y={:.3f} z={:.3f}".format(ut_x, ut_y, ut_z))
    time.sleep_ms(500)
