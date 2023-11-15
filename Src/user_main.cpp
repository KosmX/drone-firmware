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
#include "sensor/builtin_sensor_data.h"



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

        // first, init telemetry
        tlm::ITelemetry::INSTANCE = new tlm::UartTelemetry(huart8); // send telemetry to UART

        dev::init(); // init devices


        init_tasks(); // Start task threads
        sensor::sensors_init();

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

        if (sensor::bmmData.hasNew()) {
            auto r = sensor::bmmData.get().value();

            log("magnetic vector:\t" + std::to_string(r.x) + "\t" + std::to_string(r.y) + "\t" + std::to_string(r.z) + "\n");
        }

        {
            auto r = dev::bmi->getData();
            auto& a = r.first;
            auto& g = r.second;

            log("accel:\t" + std::to_string(a.x) + "\t" + std::to_string(a.y) + "\t" + std::to_string(a.z) + "\n");
            log("gyro:\t" + std::to_string(g.x) + "\t" + std::to_string(g.y) + "\t" + std::to_string(g.z) + "\n");
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

