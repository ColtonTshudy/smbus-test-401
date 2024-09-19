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

SMBus_Status SMBus_ReadByte(uint8_t, uint8_t, uint8_t *);
SMBus_Status SMBus_ReadWord(uint8_t, uint8_t, uint16_t *);
SMBus_Status SMBus_BlockRead(uint8_t, uint8_t, uint8_t *, uint8_t *);
void SMBus_Set_Speed(uint32_t);
void SMBus_ReInit();
SMBus_Status SMBus_GetStatus();