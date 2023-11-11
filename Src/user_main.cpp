//
// Created by kosmx on 10/18/23.
//

#include <string>
#include "user_main.h"
#include "os/tasks.h"
#include "telemetry.h"
#include "driver/bmp.h"
#include "devices.h"
#include "task.h"
#include "timers.h"
#include "sensor/i2c_data.h"



namespace entry {

    const TickType_t frequency = pdMS_TO_TICKS(10);
    TickType_t mainLastWakeTime;



    extern "C" void StartDefaultTask(void *argument) {
        init();

        //*
        while (true) {
            rtLoop();
        }//*/
    }


    void init() {
        // Init after os start

        dev::init(); // init devices


        init_tasks(); // Start task threads
        sensor::i2c_init();

        mainLastWakeTime = xTaskGetTickCount();
    }

    void rtLoop() {

        static uint32_t counter = 0;


        dev::pcb_led.toggle();

        if (sensor::bmpData.hasNew()) {

            auto r = sensor::bmpData.get().value(); // just make it shorter

            //sprintf(msg.get(), "pressure: %f, temp: %f\n", r.first, r.second);
            log("pressure: " + std::to_string(r.first) + ", temp: " + std::to_string(r.second) + "\n");
            //HAL_UART_Transmit_DMA(&huart8, reinterpret_cast<const uint8_t *>(msg.c_str()), msg.size());
        }


        //log("main loop took " + std::to_string((xTaskGetTickCount() - mainLastWakeTime)/(float)frequency) + " time to run\n");

        counter++;
        if (counter >= 4) counter = 0;
        vTaskDelayUntil(&mainLastWakeTime, frequency);
        //vTaskDelay(frequency);
    }
}


void preInit() {
    // Init code before OS starts
    //HAL_Delay(2000); // wait 2 seconds for debugger,
}

