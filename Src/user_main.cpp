//
// Created by kosmx on 10/18/23.
//

#include "user_main.h"
#include "gpio.h"
#include "os/tasks.h"
#include "telemetry.h"
#include "os/led.h"

os::led led{LED_D1_GPIO_Port, LED_D1_Pin};

extern "C" void StartDefaultTask(void *argument) {
    init();

    while (true) {
        rtLoop();
    }
}

void preInit() {
    // Init code before OS starts
    //HAL_Delay(2000); // wait 2 seconds for debugger,
}

void init() {
    // Init after os start
    init_tasks(); // Start task threads
}

void rtLoop() {

    static std::string msg = "hello\n";

    static bool on = false;
    led.toggle();
    log("hello\n");
    //HAL_UART_Transmit_DMA(&huart8, reinterpret_cast<const uint8_t *>(msg.c_str()), msg.size());

    on = !on;
    vTaskDelay(pdMS_TO_TICKS(500));
}
