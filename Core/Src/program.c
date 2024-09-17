#include "program.h"
#include "smbus.h"

uint8_t key = 0;
extern I2C_HandleTypeDef hi2c1;

uint8_t tx_data[2] = {0, 0};
uint8_t rx_data[2] = {0, 0};

uint16_t charge_mv = 0;

void setup()
{
    SMBus_Setup();
}

void loop()
{
    key = !HAL_GPIO_ReadPin(KEY_GPIO_Port, KEY_Pin);
    mainFSM();
}

void mainFSM()
{
    static uint8_t state = 0;
    static uint8_t desiredState = 0;

    switch (state)
    {
    case 0:
        if (key)
        {
            SMBus_Status rc;
            rc = SMBus_ReadWord(0x0B, 0x15, &charge_mv);
            if (rc != SMBUS_OK)
            {
                HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, 0);
            }
            state = 2;
            desiredState = 1;
            HAL_Delay(10);
        }
        break;
    case 1:
        if (key)
        {
            state = 2;
            desiredState = 0;
            HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, 1);
        }
        break;
    case 2:
        if (!key)
        {
            state = desiredState;
        }
    default:
        break;
    }
}

// doesn't work?
// uint8_t debounce(GPIO_TypeDef *port, uint16_t pin)
// {
//     static uint8_t state = 0;
//     static uint32_t startTime = 0;

//     switch (state)
//     {
//     case 0b00: // LOW
//         if (HAL_GPIO_ReadPin(port, pin))
//         {
//             state = 0b10;
//             startTime = HAL_GetTick();
//         }
//         break;
//     case 0b11: // HIGH
//         if (!HAL_GPIO_ReadPin(port, pin))
//         {
//             state = 0b01;
//             startTime = HAL_GetTick();
//         }
//         break;
//     case 0b10: // LOW TO HIGH TRANSITION
//         if (HAL_GetTick() - startTime > DEBOUNCE)
//         {
//             state = 0b11;
//         }
//         if (!HAL_GPIO_ReadPin(port, pin))
//         {
//             state = 0b00;
//         }
//         break;
//     case 0b01: // HIGH TO LOW TRANSITION
//         if (HAL_GetTick() - startTime > DEBOUNCE)
//         {
//             state = 0b00;
//         }
//         if (!HAL_GPIO_ReadPin(port, pin))
//         {
//             state = 0b11;
//         }
//         break;
//     default:
//         break;
//     }

//     return state & 0b01;
// }