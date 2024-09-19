#include "task_smbus.h"
#include "main.h"

SMBus_Status smbus_status;
extern I2C_HandleTypeDef hi2c1;
uint32_t speed = 20000;

void SMBus_Set_Speed(uint32_t newSpeed)
{
    hi2c1.Init.ClockSpeed = newSpeed;
    SMBus_ReInit();
}

void SMBus_ReInit()
{
    HAL_I2C_DeInit(&hi2c1);
    if (HAL_I2C_Init(&hi2c1) != HAL_OK)
    {
        Error_Handler();
    }
    smbus_status = SMBUS_OK;
}

SMBus_Status SMBus_GetStatus()
{
    return smbus_status;
}

SMBus_Status SMBus_ReadByte(uint8_t deviceAddress, uint8_t command, uint8_t *data)
{
    SMBus_Status status = SMBUS_OK;

    // Check if data pointer is valid
    if (data == NULL)
    {
        return SMBUS_ERROR_DATA_NACK;
    }

    // Wait until I2C peripheral is not busy
    WAIT_FOR_FLAG(!(I2C1->SR2 & I2C_SR2_BUSY), SMBUS_ERROR_BUS_BUSY);

    // Generate START condition
    I2C1->CR1 |= I2C_CR1_START;
    WAIT_FOR_FLAG(I2C1->SR1 & I2C_SR1_SB, SMBUS_ERROR_TIMEOUT);

    // Send device address (WRITE)
    I2C1->DR = (deviceAddress << 1) & 0xFE;
    WAIT_FOR_FLAG(I2C1->SR1 & I2C_SR1_ADDR, SMBUS_ERROR_ADDR_NACK);
    (void)I2C1->SR2; // Clear ADDR flag

    // Send command
    I2C1->DR = command;
    WAIT_FOR_FLAG(I2C1->SR1 & I2C_SR1_TXE, SMBUS_ERROR_TIMEOUT);

    // Generate REPEATED START condition
    I2C1->CR1 |= I2C_CR1_START;
    WAIT_FOR_FLAG(I2C1->SR1 & I2C_SR1_SB, SMBUS_ERROR_TIMEOUT);

    // Send device address (READ)
    I2C1->DR = (deviceAddress << 1) | 0x01;
    WAIT_FOR_FLAG(I2C1->SR1 & I2C_SR1_ADDR, SMBUS_ERROR_ADDR_NACK);
    (void)I2C1->SR2; // Clear ADDR flag

    // Disable ACK (for NACK on last byte)
    I2C1->CR1 &= ~I2C_CR1_ACK;

    // Read first byte (LSB)
    WAIT_FOR_FLAG(I2C1->SR1 & I2C_SR1_RXNE, SMBUS_ERROR_TIMEOUT);
    *data = I2C1->DR;

cleanup:
    // Generate STOP condition
    I2C1->CR1 |= I2C_CR1_STOP;

    smbus_status = status;
    return status;
}

SMBus_Status SMBus_ReadWord(uint8_t deviceAddress, uint8_t command, uint16_t *data)
{
    SMBus_Status status = SMBUS_OK;

    // Check if data pointer is valid
    if (data == NULL)
    {
        return SMBUS_ERROR_DATA_NACK;
    }

    // Wait until I2C peripheral is not busy
    WAIT_FOR_FLAG(!(I2C1->SR2 & I2C_SR2_BUSY), SMBUS_ERROR_BUS_BUSY);

    // Generate START condition
    I2C1->CR1 |= I2C_CR1_START;
    WAIT_FOR_FLAG(I2C1->SR1 & I2C_SR1_SB, SMBUS_ERROR_TIMEOUT);

    // Send device address (WRITE)
    I2C1->DR = (deviceAddress << 1) & 0xFE;
    WAIT_FOR_FLAG(I2C1->SR1 & I2C_SR1_ADDR, SMBUS_ERROR_ADDR_NACK);
    (void)I2C1->SR2; // Clear ADDR flag

    // Send command
    I2C1->DR = command;
    WAIT_FOR_FLAG(I2C1->SR1 & I2C_SR1_TXE, SMBUS_ERROR_TIMEOUT);

    // Generate REPEATED START condition
    I2C1->CR1 |= I2C_CR1_START;
    WAIT_FOR_FLAG(I2C1->SR1 & I2C_SR1_SB, SMBUS_ERROR_TIMEOUT);

    // Send device address (READ)
    I2C1->DR = (deviceAddress << 1) | 0x01;
    WAIT_FOR_FLAG(I2C1->SR1 & I2C_SR1_ADDR, SMBUS_ERROR_ADDR_NACK);
    (void)I2C1->SR2; // Clear ADDR flag

    // Enable ACK
    I2C1->CR1 |= I2C_CR1_ACK;

    // Read first byte (LSB)
    WAIT_FOR_FLAG(I2C1->SR1 & I2C_SR1_RXNE, SMBUS_ERROR_TIMEOUT);
    *data = I2C1->DR;

    // Disable ACK (for NACK on last byte)
    I2C1->CR1 &= ~I2C_CR1_ACK;

    // Read second byte (MSB)
    WAIT_FOR_FLAG(I2C1->SR1 & I2C_SR1_RXNE, SMBUS_ERROR_TIMEOUT);
    *data |= I2C1->DR << 8;

cleanup:
    // Generate STOP condition
    I2C1->CR1 |= I2C_CR1_STOP;

    smbus_status = status;
    return status;
}

SMBus_Status SMBus_BlockRead(uint8_t deviceAddress, uint8_t command, uint8_t *data, uint8_t *length)
{
    SMBus_Status status = SMBUS_OK;
    uint8_t bytesRead = 0;

    // Check if data and length pointers are valid
    if (data == NULL || length == NULL)
    {
        return SMBUS_ERROR_DATA_NACK;
    }

    // Wait until I2C peripheral is not busy
    WAIT_FOR_FLAG(!(I2C1->SR2 & I2C_SR2_BUSY), SMBUS_ERROR_BUS_BUSY);

    // Generate START condition
    I2C1->CR1 |= I2C_CR1_START;
    WAIT_FOR_FLAG(I2C1->SR1 & I2C_SR1_SB, SMBUS_ERROR_TIMEOUT);

    // Send device address (WRITE)
    I2C1->DR = (deviceAddress << 1) & 0xFE;
    WAIT_FOR_FLAG(I2C1->SR1 & I2C_SR1_ADDR, SMBUS_ERROR_ADDR_NACK);
    (void)I2C1->SR2; // Clear ADDR flag

    // Send command
    I2C1->DR = command;
    WAIT_FOR_FLAG(I2C1->SR1 & I2C_SR1_TXE, SMBUS_ERROR_TIMEOUT);

    // Generate REPEATED START condition
    I2C1->CR1 |= I2C_CR1_START;
    WAIT_FOR_FLAG(I2C1->SR1 & I2C_SR1_SB, SMBUS_ERROR_TIMEOUT);

    // Send device address (READ)
    I2C1->DR = (deviceAddress << 1) | 0x01;
    WAIT_FOR_FLAG(I2C1->SR1 & I2C_SR1_ADDR, SMBUS_ERROR_ADDR_NACK);
    (void)I2C1->SR2; // Clear ADDR flag

    // Enable ACK
    I2C1->CR1 |= I2C_CR1_ACK;

    // Read first byte (length)
    WAIT_FOR_FLAG(I2C1->SR1 & I2C_SR1_RXNE, SMBUS_ERROR_TIMEOUT);
    *length = I2C1->DR;

    // Read remaining bytes
    for (bytesRead = 0; bytesRead < *length; bytesRead++)
    {
        if (bytesRead == (*length - 1))
        {
            // Disable ACK for last byte
            I2C1->CR1 &= ~I2C_CR1_ACK;
        }

        WAIT_FOR_FLAG(I2C1->SR1 & I2C_SR1_RXNE, SMBUS_ERROR_TIMEOUT);
        data[bytesRead] = I2C1->DR;
    }

cleanup:
    // Generate STOP condition
    I2C1->CR1 |= I2C_CR1_STOP;

    smbus_status = status;
    return status;
}