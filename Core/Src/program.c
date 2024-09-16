#include "program.h"

uint8_t key = 0;
uint8_t state = 0;
extern SMBUS_HandleTypeDef hsmbus1;

uint8_t tx_data[2];
uint8_t rx_data[2];

void setup()
{
    HAL_StatusTypeDef rc = 0;
    rc = HAL_SMBUS_Master_Transmit_IT(&hsmbus1, ADDRESS, tx_data, 1, SMBUS_FIRST_AND_LAST_FRAME_NO_PEC);
    if (rc != HAL_OK)
    {
        printf("Failed to transmit smbus");
    }
    printf("Waiting for SMBus transmit complete");

    while (HAL_SMBUS_GetState(&hsmbus1) != HAL_SMBUS_STATE_READY)
    {
    }

    rc = HAL_SMBUS_Master_Receive_IT(&hsmbus1, ADDRESS, rx_data, 2, SMBUS_LAST_FRAME_NO_PEC);
    if (rc != HAL_OK)
    {
        printf("Failed to recieve smbus");
    }

    while (HAL_SMBUS_GetState(&hsmbus1) != HAL_SMBUS_STATE_READY)
    {
    }

    uint16_t result = (uint16_t)rx_data[0] | ((uint16_t)rx_data[1] << 8);

    printf("Device returned: %04x", result);
}

void loop()
{
    key = HAL_GPIO_ReadPin(KEY_GPIO_Port, KEY_Pin);
    mainFSM();
}

void mainFSM()
{
    switch (state)
    {
    case 0:
        if (key)
        {
            state = 1;
        }
        break;
    case 1:
        if (!key)
        {
            state = 0;
        }
        break;
    default:
        break;
    }
}