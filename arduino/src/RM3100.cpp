#include "RM3100.h"

RM3100::RM3100(SPIClass &spi, uint8_t chipSelectPin)
    : _spi(spi),
      _chipSelectPin(chipSelectPin),
      _cycleCount(200),
      _spiSettings(1000000, MSBFIRST, SPI_MODE0) {
}

bool RM3100::begin(uint16_t cycleCount) {
    pinMode(_chipSelectPin, OUTPUT);
    digitalWrite(_chipSelectPin, HIGH);
    _spi.begin();

    _cycleCount = cycleCount;
    return setCycleCountXYZ(_cycleCount);
}

bool RM3100::begin(uint16_t cycleCount, int8_t sckPin, int8_t misoPin, int8_t mosiPin) {
    pinMode(_chipSelectPin, OUTPUT);
    digitalWrite(_chipSelectPin, HIGH);
    _spi.begin(sckPin, misoPin, mosiPin, _chipSelectPin);

    _cycleCount = cycleCount;
    return setCycleCountXYZ(_cycleCount);
}

bool RM3100::isConnected() {
    const uint8_t revision = readRevision();
    return revision != 0x00 && revision != 0xFF;
}

uint8_t RM3100::readRevision() {
    uint8_t revision = 0;
    readRegisters(REG_REVID, &revision, 1);
    return revision;
}

bool RM3100::readRawXYZ(RM3100RawReading &reading) {
    uint8_t raw[9] = {0};

    writeRegister(REG_POLL, POLL_XYZ);
    if (!waitForDataReady(100)) {
        return false;
    }

    readRegisters(REG_MX, raw, sizeof(raw));

    reading.x = signExtend24(
        ((uint32_t)raw[0] << 16) |
        ((uint32_t)raw[1] << 8) |
        raw[2]);
    reading.y = signExtend24(
        ((uint32_t)raw[3] << 16) |
        ((uint32_t)raw[4] << 8) |
        raw[5]);
    reading.z = signExtend24(
        ((uint32_t)raw[6] << 16) |
        ((uint32_t)raw[7] << 8) |
        raw[8]);

    return true;
}

bool RM3100::readFieldUT(RM3100FieldReading &reading) {
    RM3100RawReading rawReading;

    if (!readRawXYZ(rawReading)) {
        return false;
    }

    convertRawToFieldUT(rawReading, reading);
    return true;
}

void RM3100::convertRawToFieldUT(const RM3100RawReading &rawReading, RM3100FieldReading &fieldReading) const {
    const float gain = gainLsbPerUT();

    fieldReading.x_uT = rawReading.x / gain;
    fieldReading.y_uT = rawReading.y / gain;
    fieldReading.z_uT = rawReading.z / gain;
}

uint16_t RM3100::cycleCount() const {
    return _cycleCount;
}

float RM3100::gainLsbPerUT() const {
    return gainFromCycleCount(_cycleCount);
}

void RM3100::select() {
    _spi.beginTransaction(_spiSettings);
    digitalWrite(_chipSelectPin, LOW);
}

void RM3100::deselect() {
    digitalWrite(_chipSelectPin, HIGH);
    _spi.endTransaction();
}

void RM3100::writeRegister(uint8_t reg, uint8_t value) {
    select();
    _spi.transfer(reg);
    _spi.transfer(value);
    deselect();
}

void RM3100::writeRegisters(uint8_t startReg, const uint8_t *data, size_t length) {
    select();
    _spi.transfer(startReg);
    for (size_t index = 0; index < length; ++index) {
        _spi.transfer(data[index]);
    }
    deselect();
}

void RM3100::readRegisters(uint8_t startReg, uint8_t *data, size_t length) {
    select();
    _spi.transfer(READ_BIT | startReg);
    for (size_t index = 0; index < length; ++index) {
        data[index] = _spi.transfer(0x00);
    }
    deselect();
}

bool RM3100::waitForDataReady(uint32_t timeoutMs) {
    const uint32_t startedAt = millis();
    uint8_t status = 0;

    do {
        readRegisters(REG_STATUS, &status, 1);
        if ((status & STATUS_DRDY) != 0) {
            return true;
        }
        delay(1);
    } while ((millis() - startedAt) < timeoutMs);

    return false;
}

bool RM3100::setCycleCountXYZ(uint16_t cycleCount) {
    uint8_t buffer[6] = {
        (uint8_t)(cycleCount >> 8),
        (uint8_t)(cycleCount & 0xFF),
        (uint8_t)(cycleCount >> 8),
        (uint8_t)(cycleCount & 0xFF),
        (uint8_t)(cycleCount >> 8),
        (uint8_t)(cycleCount & 0xFF)
    };

    writeRegisters(REG_CCX, buffer, sizeof(buffer));
    return true;
}

int32_t RM3100::signExtend24(uint32_t raw) {
    if ((raw & 0x00800000UL) != 0) {
        raw |= 0xFF000000UL;
    }
    return (int32_t)raw;
}

float RM3100::gainFromCycleCount(uint16_t cycleCount) {
    switch (cycleCount) {
        case 50:
            return 20.0f;
        case 100:
            return 38.0f;
        case 200:
            return 75.0f;
        default:
            return 0.375f * cycleCount;
    }
}
