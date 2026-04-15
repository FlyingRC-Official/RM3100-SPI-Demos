#ifndef RM3100_H
#define RM3100_H

#include <stdbool.h>
#include <stdint.h>

#include "stm32f4xx_hal.h"

typedef struct {
    int32_t x;
    int32_t y;
    int32_t z;
} RM3100_RawReading;

typedef struct {
    float x_uT;
    float y_uT;
    float z_uT;
} RM3100_FieldReading;

typedef struct {
    SPI_HandleTypeDef *spi;
    GPIO_TypeDef *cs_port;
    uint16_t cs_pin;
    uint16_t cycle_count;
    uint32_t timeout_ms;
} RM3100_HandleTypeDef;

HAL_StatusTypeDef RM3100_Init(
    RM3100_HandleTypeDef *rm3100,
    SPI_HandleTypeDef *spi,
    GPIO_TypeDef *cs_port,
    uint16_t cs_pin,
    uint16_t cycle_count);

bool RM3100_IsConnected(RM3100_HandleTypeDef *rm3100);
HAL_StatusTypeDef RM3100_ReadRevision(RM3100_HandleTypeDef *rm3100, uint8_t *revision);
HAL_StatusTypeDef RM3100_ReadRawXYZ(RM3100_HandleTypeDef *rm3100, RM3100_RawReading *reading);
HAL_StatusTypeDef RM3100_ReadFieldUT(RM3100_HandleTypeDef *rm3100, RM3100_FieldReading *reading);
void RM3100_ConvertRawToFieldUT(
    RM3100_HandleTypeDef *rm3100,
    const RM3100_RawReading *raw_reading,
    RM3100_FieldReading *field_reading);
float RM3100_GainFromCycleCount(uint16_t cycle_count);
int32_t RM3100_SignExtend24(uint32_t raw);

#endif
