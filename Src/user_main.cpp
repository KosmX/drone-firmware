//
// Created by kosmx on 10/18/23.
//

#include "user_main.h"
#include "gpio.h"

extern "C" void StartDefaultTask(void *argument) {
    while (true) {
        rtLoop();
    }
}

void init() {
    
}

void rtLoop() {
    static bool on = false;
    HAL_GPIO_WritePin(EXTRA_PB2_GPIO_Port, EXTRA_PB2_Pin, on ? GPIO_PIN_SET : GPIO_PIN_RESET);
    on = !on;
    for (int i = 0; i < 1000000; ++i) {
        asm("isb"); // do nothing efficiently
    }
}
