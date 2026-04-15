#include <stdio.h>

#include "rm3100.h"

extern SPI_HandleTypeDef hspi1;

static RM3100_HandleTypeDef hrm3100;

static void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);
static void MX_USART2_UART_Init(void);

int main(void) {
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_SPI1_Init();
    MX_USART2_UART_Init();

    if (RM3100_Init(&hrm3100, &hspi1, GPIOA, GPIO_PIN_4, 200) != HAL_OK) {
        printf("RM3100 init failed\r\n");
        while (1) {
        }
    }

    {
        uint8_t revision = 0;
        if (RM3100_ReadRevision(&hrm3100, &revision) == HAL_OK) {
            printf("RM3100 REVID: 0x%02X\r\n", revision);
        } else {
            printf("Failed to read RM3100 revision\r\n");
        }
    }

    while (1) {
        RM3100_RawReading raw;
        RM3100_FieldReading field;

        if (RM3100_ReadRawXYZ(&hrm3100, &raw) == HAL_OK) {
            RM3100_ConvertRawToFieldUT(&hrm3100, &raw, &field);
            printf("raw x=%ld y=%ld z=%ld\r\n", raw.x, raw.y, raw.z);
            printf("uT  x=%.3f y=%.3f z=%.3f\r\n", field.x_uT, field.y_uT, field.z_uT);
        } else {
            printf("RM3100 sample read failed\r\n");
        }

        HAL_Delay(500);
    }
}

static void SystemClock_Config(void) {
    /* Fill in with CubeMX-generated clock setup for your board. */
}

static void MX_GPIO_Init(void) {
    /* Configure the RM3100 chip-select pin and any required GPIO here. */
}

static void MX_SPI1_Init(void) {
    /* Configure SPI1 for mode 0, MSB first, and <= 1 MHz. */
}

static void MX_USART2_UART_Init(void) {
    /* Configure a UART or replace printf usage with your logging path. */
}
