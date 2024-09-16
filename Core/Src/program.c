#include "program.h"

uint8_t key = 0;
uint8_t state = 0;
extern SMBUS_HandleTypeDef hsmbus1;

uint8_t tx_data[2] = {0, 0};
uint8_t rx_data[2] = {0, 0};

uint32_t entryTime = 0;

void setup()
{
    if (HAL_SMBUS_IsDeviceReady(&hsmbus1, ADDRESS, 20, 1000) != HAL_OK)
    {
        HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, 0);
    }
}

void smbusReinit()
{
    __HAL_SMBUS_DISABLE(&hsmbus1);
    __HAL_SMBUS_CLEAR_FLAG(&hsmbus1, SMBUS_FLAG_BERR | SMBUS_FLAG_ARLO | SMBUS_FLAG_AF | SMBUS_FLAG_OVR);
    hsmbus1.State = HAL_SMBUS_STATE_READY; // Reset the state manually
    HAL_SMBUS_Init(&hsmbus1);              // Reinitialize the SMBUS peripheral
}

void smbusBusyCheck()
{
    entryTime = HAL_GetTick();
    HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, 0);
    while (HAL_SMBUS_GetState(&hsmbus1) != HAL_SMBUS_STATE_READY)
    {
        // if (entryTime - HAL_GetTick() > 1000)
        // {
        //     smbusReinit();
        //     break;
        // }
    }
    HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, 1);
}

void smbusSendCommand()
{
    Debug("=====================================");
    Debug("SMBus transmitting...");
    tx_data[0] = 0x14;
    HAL_StatusTypeDef rc = 0;
    rc = HAL_SMBUS_Master_Transmit_IT(&hsmbus1, ADDRESS, &(tx_data[0]), 1, SMBUS_FIRST_FRAME);
    if (rc != HAL_OK)
    {
        Debug("Failed to transmit smbus");
    }
    // smbusBusyCheck();
    Debug("Transmit complete");

    Debug("SMBus receiving...");
    rc = HAL_SMBUS_Master_Receive_IT(&hsmbus1, ADDRESS, rx_data, 2, SMBUS_LAST_FRAME_NO_PEC);
    if (rc != HAL_OK)
    {
        Debug("Failed to recieve smbus");
    }
    smbusBusyCheck();
    Debug("Receive complete.");

    uint16_t result = (uint16_t)rx_data[0] | ((uint16_t)rx_data[1] << 8);

    Debug("Device returned: %04x", result);
}

void loop()
{
    key = !HAL_GPIO_ReadPin(KEY_GPIO_Port, KEY_Pin);
    mainFSM();
}

void mainFSM()
{
    switch (state)
    {
    case 0:
        if (key)
        {
            // HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, 1);
            smbusSendCommand();
            state = 1;
            HAL_Delay(10);
        }
        break;
    case 1:
        if (!key)
        {
            // HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, 0);
            state = 0;
        }
        break;
    default:
        break;
    }
}