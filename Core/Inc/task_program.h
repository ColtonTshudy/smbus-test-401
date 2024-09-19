#pragma once

#include "main.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx.h"
#include "task_usb.h"
#include "task_smbus.h"
#include <stdio.h>

#define ADDRESS (0x0B << 1) // Shifted left by 1 as per HAL convention
#define REGISTER_ADDRESS 0x14
#define DEBOUNCE 10 // ms

#define ENABLE_DEBUG 0
#if ENABLE_DEBUG
#define Debug(__info, ...) printf(" [DBG]: " __info "\r\n", ##__VA_ARGS__)
#else
#define Debug(__info, ...)
#endif

void setup();
void loop();
void mainFSM();
uint8_t debounce(GPIO_TypeDef *, uint16_t);