#pragma once

#include "main.h"
#include "stm32f4xx_hal.h"
#include <stdio.h>

#define ADDRESS (0x0B << 1) // Shifted left by 1 as per HAL convention
#define REGISTER_ADDRESS 0x14

void setup();
void loop();
void mainFSM();