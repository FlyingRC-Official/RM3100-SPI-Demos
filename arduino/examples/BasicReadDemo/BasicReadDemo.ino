#include <RM3100.h>

#if !defined(ARDUINO_ARCH_ESP32)
#warning "This example is configured for ESP32 3.3 V boards. Do not connect a 3.3 V RM3100 module directly to 5 V Arduino IO without level shifting."
#endif

// ESP32 VSPI-style default pins. Change these if your ESP32 board uses other GPIOs.
constexpr int8_t RM3100_SCK_PIN = 18;
constexpr int8_t RM3100_MISO_PIN = 19;
constexpr int8_t RM3100_MOSI_PIN = 23;
constexpr uint8_t RM3100_CS_PIN = 5;

// RM3100 uses 3.3 V IO. ESP32 is a safer default host than a classic 5 V Arduino UNO.
// Wire RM3100 I2CEN low so the module is in SPI mode.
RM3100 sensor(SPI, RM3100_CS_PIN);

void printRaw(const RM3100RawReading &reading) {
    Serial.print("raw x=");
    Serial.print(reading.x);
    Serial.print(" y=");
    Serial.print(reading.y);
    Serial.print(" z=");
    Serial.println(reading.z);
}

void printField(const RM3100FieldReading &reading) {
    Serial.print("uT  x=");
    Serial.print(reading.x_uT, 3);
    Serial.print(" y=");
    Serial.print(reading.y_uT, 3);
    Serial.print(" z=");
    Serial.println(reading.z_uT, 3);
}

void printRevisionCheck(uint8_t revision) {
    Serial.print("RM3100 REVID: 0x");
    if (revision < 0x10) {
        Serial.print('0');
    }
    Serial.println(revision, HEX);

    if (revision == 0x00 || revision == 0xFF) {
        Serial.println("Warning: REVID looks invalid. Check CS/SCK/MISO/MOSI, 3.3 V power, GND, and I2CEN=LOW.");
    } else if (revision < 0x21) {
        Serial.println("Warning: REVID is lower than expected for RM3100. SPI may still be miswired or reading the wrong device.");
    }
}

void setup() {
    Serial.begin(115200);
    delay(1000);

    Serial.println();
    Serial.println("RM3100 ESP32 SPI demo");
    Serial.println("Use 3.3 V power and 3.3 V IO only. Do not use 5 V Arduino IO directly.");
    Serial.println("Default pins: SCK=18, MISO=19, MOSI=23, CS=5");

    if (!sensor.begin(200, RM3100_SCK_PIN, RM3100_MISO_PIN, RM3100_MOSI_PIN)) {
        Serial.println("Failed to configure RM3100 cycle count.");
        while (true) {
            delay(1000);
        }
    }

    printRevisionCheck(sensor.readRevision());
    Serial.println("Reading RM3100 data over SPI...");
}

void loop() {
    RM3100RawReading rawReading;
    RM3100FieldReading fieldReading;

    if (!sensor.readRawXYZ(rawReading)) {
        Serial.println("Timed out waiting for RM3100 data ready. Check SPI wiring and I2CEN=LOW.");
        delay(500);
        return;
    }

    printRaw(rawReading);
    sensor.convertRawToFieldUT(rawReading, fieldReading);
    printField(fieldReading);

    delay(500);
}
