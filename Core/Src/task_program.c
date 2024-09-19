#include "task_program.h"

uint8_t key = 0;
extern I2C_HandleTypeDef hi2c1;
extern uint32_t speed; // smbus.c bus speed

uint8_t tx_data[2] = {0, 0};
uint8_t rx_data[2] = {0, 0};

uint16_t charge_mv = 0;

void setup()
{
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
        if (key) // manual fault trigger
        {
            state = 2;
            desiredState = 1;
            HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, 0);
        }
        if (SMBus_GetStatus() != SMBUS_OK) // fault trigger
        {
            state = 1;
            desiredState = 1;
            HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, 0);
        }
        break;

    case 1:
        if (key) // manual fault reset
        {
            state = 2;
            desiredState = 0;
        }
        break;

    case 2: // captures rising edge of keypress, switches state on falling edge
        if (!key)
        {
            state = desiredState;
            if (desiredState == 0)
            {
                HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, 1);
                SMBus_ReInit();
            }
        }
        break;

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