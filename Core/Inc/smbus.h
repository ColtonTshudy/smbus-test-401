#pragma once

#include "stm32f4xx_hal.h"

typedef enum
{
    SMBUS_OK = 0,
    SMBUS_ERROR_TIMEOUT,
    SMBUS_ERROR_BUS_BUSY,
    SMBUS_ERROR_ADDR_NACK,
    SMBUS_ERROR_DATA_NACK
} SMBus_Status;

#define SPEED 50000          // Hz
#define SMBUS_TIMEOUT 840000 // clock cycles

#define WAIT_FOR_FLAG(flag, error_code)   \
    do                                    \
    {                                     \
        uint32_t timeout = SMBUS_TIMEOUT; \
        while (!(flag) && --timeout)      \
            ;                             \
        if (timeout == 0)                 \
        {                                 \
            status = error_code;          \
            goto cleanup;                 \
        }                                 \
    } while (0)

void SMBus_Setup();
SMBus_Status SMBus_ReadWord(uint8_t, uint8_t, uint16_t *);