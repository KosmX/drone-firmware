//
// Created by kosmx on 10/18/23.
//

#include <string>
#include "user_main.h"
#include "os/tasks.h"
#include "telemetry.h"
#include "driver/bmp.h"
#include "devices.h"


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

    dev::init(); // init devices

    init_tasks(); // Start task threads
}

void rtLoop() {

    static bool on = false;
    dev::pcb_led.toggle();

    auto r = dev::bmp->activateAndRead();
    //sprintf(msg.get(), "pressure: %f, temp: %f\n", r.first, r.second);
    log("pressure: " + std::to_string(r.first) + ", temp: " + std::to_string(r.second) + "\n");
    //HAL_UART_Transmit_DMA(&huart8, reinterpret_cast<const uint8_t *>(msg.c_str()), msg.size());

    on = !on;
    vTaskDelay(pdMS_TO_TICKS(20));
}
