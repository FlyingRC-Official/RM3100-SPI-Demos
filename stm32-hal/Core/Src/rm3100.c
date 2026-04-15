#include "rm3100.h"

enum {
    RM3100_REG_POLL = 0x00,
    RM3100_REG_CCX = 0x04,
    RM3100_REG_STATUS = 0x34,
    RM3100_REG_REVID = 0x36,
    RM3100_REG_MX = 0x24,
    RM3100_READ_BIT = 0x80,
    RM3100_POLL_XYZ = 0x70,
    RM3100_STATUS_DRDY = 0x80
};

static void RM3100_Select(RM3100_HandleTypeDef *rm3100) {
    HAL_GPIO_WritePin(rm3100->cs_port, rm3100->cs_pin, GPIO_PIN_RESET);
}

static void RM3100_Deselect(RM3100_HandleTypeDef *rm3100) {
    HAL_GPIO_WritePin(rm3100->cs_port, rm3100->cs_pin, GPIO_PIN_SET);
}

static HAL_StatusTypeDef RM3100_WriteRegister(RM3100_HandleTypeDef *rm3100, uint8_t reg, uint8_t value) {
    uint8_t tx[2] = {reg, value};

    RM3100_Select(rm3100);
    HAL_StatusTypeDef status = HAL_SPI_Transmit(rm3100->spi, tx, 2, rm3100->timeout_ms);
    RM3100_Deselect(rm3100);
    return status;
}

static HAL_StatusTypeDef RM3100_WriteRegisters(
    RM3100_HandleTypeDef *rm3100,
    uint8_t start_reg,
    const uint8_t *data,
    uint16_t length) {
    HAL_StatusTypeDef status;

    RM3100_Select(rm3100);
    status = HAL_SPI_Transmit(rm3100->spi, &start_reg, 1, rm3100->timeout_ms);
    if (status == HAL_OK) {
        status = HAL_SPI_Transmit(rm3100->spi, (uint8_t *)data, length, rm3100->timeout_ms);
    }
    RM3100_Deselect(rm3100);
    return status;
}

static HAL_StatusTypeDef RM3100_ReadRegisters(
    RM3100_HandleTypeDef *rm3100,
    uint8_t start_reg,
    uint8_t *data,
    uint16_t length) {
    const uint8_t command = (uint8_t)(RM3100_READ_BIT | start_reg);
    HAL_StatusTypeDef status;

    RM3100_Select(rm3100);
    status = HAL_SPI_Transmit(rm3100->spi, (uint8_t *)&command, 1, rm3100->timeout_ms);
    if (status == HAL_OK) {
        status = HAL_SPI_Receive(rm3100->spi, data, length, rm3100->timeout_ms);
    }
    RM3100_Deselect(rm3100);
    return status;
}

static HAL_StatusTypeDef RM3100_SetCycleCountXYZ(RM3100_HandleTypeDef *rm3100, uint16_t cycle_count) {
    uint8_t buffer[6] = {
        (uint8_t)(cycle_count >> 8),
        (uint8_t)(cycle_count & 0xFF),
        (uint8_t)(cycle_count >> 8),
        (uint8_t)(cycle_count & 0xFF),
        (uint8_t)(cycle_count >> 8),
        (uint8_t)(cycle_count & 0xFF)
    };

    rm3100->cycle_count = cycle_count;
    return RM3100_WriteRegisters(rm3100, RM3100_REG_CCX, buffer, sizeof(buffer));
}

static HAL_StatusTypeDef RM3100_WaitForDataReady(RM3100_HandleTypeDef *rm3100) {
    uint8_t status = 0;
    const uint32_t started_at = HAL_GetTick();

    do {
        HAL_StatusTypeDef io_status = RM3100_ReadRegisters(rm3100, RM3100_REG_STATUS, &status, 1);
        if (io_status != HAL_OK) {
            return io_status;
        }
        if ((status & RM3100_STATUS_DRDY) != 0) {
            return HAL_OK;
        }
        HAL_Delay(1);
    } while ((HAL_GetTick() - started_at) < rm3100->timeout_ms);

    return HAL_TIMEOUT;
}

HAL_StatusTypeDef RM3100_Init(
    RM3100_HandleTypeDef *rm3100,
    SPI_HandleTypeDef *spi,
    GPIO_TypeDef *cs_port,
    uint16_t cs_pin,
    uint16_t cycle_count) {
    rm3100->spi = spi;
    rm3100->cs_port = cs_port;
    rm3100->cs_pin = cs_pin;
    rm3100->timeout_ms = 100;
    rm3100->cycle_count = cycle_count;

    RM3100_Deselect(rm3100);
    return RM3100_SetCycleCountXYZ(rm3100, cycle_count);
}

bool RM3100_IsConnected(RM3100_HandleTypeDef *rm3100) {
    uint8_t revision = 0;

    if (RM3100_ReadRevision(rm3100, &revision) != HAL_OK) {
        return false;
    }

    return revision != 0x00 && revision != 0xFF;
}

HAL_StatusTypeDef RM3100_ReadRevision(RM3100_HandleTypeDef *rm3100, uint8_t *revision) {
    return RM3100_ReadRegisters(rm3100, RM3100_REG_REVID, revision, 1);
}

HAL_StatusTypeDef RM3100_ReadRawXYZ(RM3100_HandleTypeDef *rm3100, RM3100_RawReading *reading) {
    uint8_t raw[9];
    HAL_StatusTypeDef status;

    status = RM3100_WriteRegister(rm3100, RM3100_REG_POLL, RM3100_POLL_XYZ);
    if (status != HAL_OK) {
        return status;
    }

    status = RM3100_WaitForDataReady(rm3100);
    if (status != HAL_OK) {
        return status;
    }

    status = RM3100_ReadRegisters(rm3100, RM3100_REG_MX, raw, sizeof(raw));
    if (status != HAL_OK) {
        return status;
    }

    reading->x = RM3100_SignExtend24(
        ((uint32_t)raw[0] << 16) |
        ((uint32_t)raw[1] << 8) |
        raw[2]);
    reading->y = RM3100_SignExtend24(
        ((uint32_t)raw[3] << 16) |
        ((uint32_t)raw[4] << 8) |
        raw[5]);
    reading->z = RM3100_SignExtend24(
        ((uint32_t)raw[6] << 16) |
        ((uint32_t)raw[7] << 8) |
        raw[8]);

    return HAL_OK;
}

HAL_StatusTypeDef RM3100_ReadFieldUT(RM3100_HandleTypeDef *rm3100, RM3100_FieldReading *reading) {
    RM3100_RawReading raw_reading;
    HAL_StatusTypeDef status = RM3100_ReadRawXYZ(rm3100, &raw_reading);

    if (status != HAL_OK) {
        return status;
    }

    RM3100_ConvertRawToFieldUT(rm3100, &raw_reading, reading);
    return HAL_OK;
}

void RM3100_ConvertRawToFieldUT(
    RM3100_HandleTypeDef *rm3100,
    const RM3100_RawReading *raw_reading,
    RM3100_FieldReading *field_reading) {
    const float gain = RM3100_GainFromCycleCount(rm3100->cycle_count);

    field_reading->x_uT = raw_reading->x / gain;
    field_reading->y_uT = raw_reading->y / gain;
    field_reading->z_uT = raw_reading->z / gain;
}

float RM3100_GainFromCycleCount(uint16_t cycle_count) {
    switch (cycle_count) {
        case 50:
            return 20.0f;
        case 100:
            return 38.0f;
        case 200:
            return 75.0f;
        default:
            return 0.375f * cycle_count;
    }
}

int32_t RM3100_SignExtend24(uint32_t raw) {
    if ((raw & 0x00800000UL) != 0UL) {
        raw |= 0xFF000000UL;
    }
    return (int32_t)raw;
}
