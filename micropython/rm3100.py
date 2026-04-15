class RM3100:
    REG_POLL = 0x00
    REG_CCX = 0x04
    REG_STATUS = 0x34
    REG_REVID = 0x36
    REG_MX = 0x24

    READ_BIT = 0x80
    POLL_XYZ = 0x70
    STATUS_DRDY = 0x80

    def __init__(self, spi, cs_pin, cycle_count=200):
        self.spi = spi
        self.cs = cs_pin
        self.cycle_count = cycle_count
        self.cs.value(1)

    def begin(self):
        self.set_cycle_count_xyz(self.cycle_count)

    def read_revision(self):
        return self.read_registers(self.REG_REVID, 1)[0]

    def is_connected(self):
        revision = self.read_revision()
        return revision not in (0x00, 0xFF)

    def read_raw_xyz(self):
        self.write_register(self.REG_POLL, self.POLL_XYZ)
        self.wait_for_data_ready()

        raw = self.read_registers(self.REG_MX, 9)
        x = self.sign_extend_24((raw[0] << 16) | (raw[1] << 8) | raw[2])
        y = self.sign_extend_24((raw[3] << 16) | (raw[4] << 8) | raw[5])
        z = self.sign_extend_24((raw[6] << 16) | (raw[7] << 8) | raw[8])
        return (x, y, z)

    def read_ut_xyz(self):
        return self.convert_raw_to_ut(self.read_raw_xyz())

    def convert_raw_to_ut(self, raw_xyz):
        gain = self.gain_from_cycle_count(self.cycle_count)
        x, y, z = raw_xyz
        return (x / gain, y / gain, z / gain)

    def write_register(self, reg, value):
        self.cs.value(0)
        self.spi.write(bytes((reg, value)))
        self.cs.value(1)

    def write_registers(self, start_reg, values):
        self.cs.value(0)
        self.spi.write(bytes((start_reg,)))
        self.spi.write(values)
        self.cs.value(1)

    def read_registers(self, start_reg, length):
        data = bytearray(length)
        self.cs.value(0)
        self.spi.write(bytes((self.READ_BIT | start_reg,)))
        self.spi.readinto(data)
        self.cs.value(1)
        return data

    def set_cycle_count_xyz(self, cycle_count):
        self.cycle_count = cycle_count
        payload = bytes((
            (cycle_count >> 8) & 0xFF,
            cycle_count & 0xFF,
            (cycle_count >> 8) & 0xFF,
            cycle_count & 0xFF,
            (cycle_count >> 8) & 0xFF,
            cycle_count & 0xFF,
        ))
        self.write_registers(self.REG_CCX, payload)

    def wait_for_data_ready(self, timeout_ms=100):
        import time

        started_at = time.ticks_ms()
        while time.ticks_diff(time.ticks_ms(), started_at) < timeout_ms:
            status = self.read_registers(self.REG_STATUS, 1)[0]
            if status & self.STATUS_DRDY:
                return
            time.sleep_ms(1)
        raise OSError("RM3100 data ready timeout")

    @staticmethod
    def sign_extend_24(raw):
        if raw & 0x00800000:
            raw |= 0xFF000000
        if raw & 0x80000000:
            raw -= 0x100000000
        return raw

    @staticmethod
    def gain_from_cycle_count(cycle_count):
        if cycle_count == 50:
            return 20.0
        if cycle_count == 100:
            return 38.0
        if cycle_count == 200:
            return 75.0
        return 0.375 * cycle_count
