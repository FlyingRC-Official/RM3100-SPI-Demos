#include <RM3100.h>

constexpr uint8_t RM3100_CS_PIN = 10;

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

void setup() {
    Serial.begin(115200);
    while (!Serial) {
    }

    if (!sensor.begin(200)) {
        Serial.println("Failed to configure RM3100 cycle count.");
        while (true) {
        }
    }

    Serial.print("RM3100 REVID: 0x");
    Serial.println(sensor.readRevision(), HEX);
    Serial.println("Reading RM3100 data over SPI...");
}

void loop() {
    RM3100RawReading rawReading;
    RM3100FieldReading fieldReading;

    if (!sensor.readRawXYZ(rawReading)) {
        Serial.println("Timed out waiting for RM3100 data ready.");
        delay(500);
        return;
    }

    printRaw(rawReading);
    sensor.convertRawToFieldUT(rawReading, fieldReading);
    printField(fieldReading);

    delay(500);
}
