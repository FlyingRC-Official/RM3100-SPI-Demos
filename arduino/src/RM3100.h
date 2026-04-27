#ifndef RM3100_H
#define RM3100_H

#include <Arduino.h>
#include <SPI.h>

struct RM3100RawReading {
    int32_t x;
    int32_t y;
    int32_t z;
};

struct RM3100FieldReading {
    float x_uT;
    float y_uT;
    float z_uT;
};

class RM3100 {
public:
    RM3100(SPIClass &spi, uint8_t chipSelectPin);

    bool begin(uint16_t cycleCount = 200);
    bool begin(uint16_t cycleCount, int8_t sckPin, int8_t misoPin, int8_t mosiPin);
    bool isConnected();
    uint8_t readRevision();

    bool readRawXYZ(RM3100RawReading &reading);
    bool readFieldUT(RM3100FieldReading &reading);
    void convertRawToFieldUT(const RM3100RawReading &rawReading, RM3100FieldReading &fieldReading) const;

    uint16_t cycleCount() const;
    float gainLsbPerUT() const;

private:
    static constexpr uint8_t REG_POLL = 0x00;
    static constexpr uint8_t REG_CCX = 0x04;
    static constexpr uint8_t REG_STATUS = 0x34;
    static constexpr uint8_t REG_REVID = 0x36;
    static constexpr uint8_t REG_MX = 0x24;
    static constexpr uint8_t READ_BIT = 0x80;
    static constexpr uint8_t POLL_XYZ = 0x70;
    static constexpr uint8_t STATUS_DRDY = 0x80;

    SPIClass &_spi;
    uint8_t _chipSelectPin;
    uint16_t _cycleCount;
    SPISettings _spiSettings;

    void select();
    void deselect();
    void writeRegister(uint8_t reg, uint8_t value);
    void writeRegisters(uint8_t startReg, const uint8_t *data, size_t length);
    void readRegisters(uint8_t startReg, uint8_t *data, size_t length);
    bool waitForDataReady(uint32_t timeoutMs);
    bool setCycleCountXYZ(uint16_t cycleCount);

    static int32_t signExtend24(uint32_t raw);
    static float gainFromCycleCount(uint16_t cycleCount);
};

#endif
