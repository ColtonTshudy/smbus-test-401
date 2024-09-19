#include "task_smbus.h"
#include "main.h"

extern I2C_HandleTypeDef hi2c1;
uint32_t speed = 20000;

void SMBus_Incr_Speed()
{
    HAL_I2C_DeInit(&hi2c1);
    hi2c1.Init.ClockSpeed = speed;
    if (HAL_I2C_Init(&hi2c1) != HAL_OK)
    {
        Error_Handler();
    }
    speed += 20000;

    if (speed > 200000)
    {
        speed = 0;
    }
}

void SMBus_Set_Speed(uint32_t newSpeed)
{
    HAL_I2C_DeInit(&hi2c1);
    hi2c1.Init.ClockSpeed = newSpeed;
    if (HAL_I2C_Init(&hi2c1) != HAL_OK)
    {
        Error_Handler();
    }
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

    // Read first byte (MSB)
    WAIT_FOR_FLAG(I2C1->SR1 & I2C_SR1_RXNE, SMBUS_ERROR_TIMEOUT);
    *data = (I2C1->DR << 8);

    // Disable ACK (for NACK on last byte)
    I2C1->CR1 &= ~I2C_CR1_ACK;

    // Read second byte (LSB)
    WAIT_FOR_FLAG(I2C1->SR1 & I2C_SR1_RXNE, SMBUS_ERROR_TIMEOUT);
    *data |= I2C1->DR;

cleanup:
    // Generate STOP condition
    I2C1->CR1 |= I2C_CR1_STOP;

    return status;
}