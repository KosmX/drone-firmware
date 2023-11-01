//
// Created by kosmx on 10/18/23.
//

#include "user_main.h"
#include "gpio.h"
#include "tasks.h"
#include "telemetry.h"

extern "C" void StartDefaultTask(void *argument) {
    init();

    while (true) {
        rtLoop();
    }
}

void preInit() {
    // Init code before OS starts
}

void init() {
    // Init after os start
    init_tasks(); // Start task threads
}

void rtLoop() {

    static std::string msg = "hello\n";

    static bool on = false;
    HAL_GPIO_WritePin(LED_D1_GPIO_Port, LED_D1_Pin, on ? GPIO_PIN_SET : GPIO_PIN_RESET);
    //log("hello");
    HAL_UART_Transmit_DMA(&huart8, reinterpret_cast<const uint8_t *>(msg.c_str()), msg.size());

    on = !on;
    vTaskDelay(pdMS_TO_TICKS(500));
}
