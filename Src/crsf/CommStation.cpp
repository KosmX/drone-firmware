//
// Created by kosmx on 4/5/24.
//

#include "CommStation.h"

CommStation* CommStation::INSTANCE = nullptr;

namespace crsf {


    constexpr const osThreadAttr_t thread_attr = {
            .name = "telemetry",
            .stack_size = 128 * 4,
            .priority = (osPriority_t) osPriorityNormal,
    };


    ELRSController::ELRSController(os::uart_dma &uart): uartInstance(uart) {
        handleThread = osThreadNew([](void* p) {
            auto* pThis = reinterpret_cast<ELRSController*>(p);
            pThis->runThread();

        }, this, &thread_attr);
    }

    /**
     * Thread function
     */
    [[noreturn]] void ELRSController::runThread() {

        const TickType_t frequency = pdMS_TO_TICKS(10);
        TickType_t lastWakeTime;


        while (true) {




            // delay before doing everything again
            vTaskDelayUntil(&lastWakeTime, frequency);
        }
    }
} // crsf