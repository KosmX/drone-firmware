//
// Created by kosmx on 10/18/23.
//

#include <string>
#include <format>
#include "user_main.h"
#include "os/tasks.h"
#include "os/Task.h"
#include "telemetry.h"
#include "driver/bmp.h"
#include "devices.h"
#include "task.h"
#include "timers.h"
#include "sensor/builtin_sensor_data.h"
#include "crsf/CommStation.h"
#include "control.h"
#include "driver/dshot.h"



namespace entry {

    const TickType_t frequency = pdMS_TO_TICKS(2);
    TickType_t mainLastWakeTime;


    extern "C" void StartDefaultTask(void *argument) {
        dev::pcb_led.setState(true);

        vTaskDelay(2000);
        init();

        dev::pcb_led.setState(false);
        //*
        while (true) {
            rtLoop();
        }//*/
    }

    Control control;


    void init() {
        // Init after os start

        // first, init telemetry
        tlm::ITelemetry::INSTANCE = new tlm::UartTelemetry(huart8); // send telemetry to UART

        dev::init(); // init devices

        os::Task task{"suicideTask"};
        task.start([] {
            5 + 5; // do nothing useful then exit
        });

        tasks::init(); // Start task threads
        sensor::sensors_init();


        for (int i = 0; i < 100; i += 2) {
            dshot_write(control.motor_value);
            os::sleep(2);
        }

        mainLastWakeTime = xTaskGetTickCount();
    }

    void rtLoop() {
        static uint32_t counter = 0;

        control.controlLoop();
        if (xTaskGetTickCount() - mainLastWakeTime >= frequency) {
            log("Main loop took longer than expected");
#ifdef USE_FULL_ASSERT
            assert_param(false);
#endif
        }
        vTaskDelayUntil(&mainLastWakeTime, frequency);
    }
}


void preInit() {
    // Init code before OS starts
    //HAL_Delay(2000); // wait 2 seconds for debugger,
}

void taskError() {

    TaskStatus_t taskStatus{};

    vTaskGetInfo({}, &taskStatus, pdTRUE, eRunning);

    log(std::format("Thread {} status:\nbase priority: {}\ncurrent priority: {}\nwatermark: {}",
                    taskStatus.pcTaskName, taskStatus.uxBasePriority, taskStatus.uxCurrentPriority, taskStatus.usStackHighWaterMark));

}

void user_assert_failed(uint8_t *file, uint32_t line) {

    log(std::format("Exception: file {} on line {}\n", reinterpret_cast<char*>(file), line));

    taskError();
}

