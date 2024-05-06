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

    const TickType_t frequency = pdMS_TO_TICKS(10);
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
            5+5; // do nothing useful then exit
        });

        tasks::init(); // Start task threads
        sensor::sensors_init();


        control.motor_value[0] = 1;
        dshot_write(control.motor_value);
        os::sleep(260);
        control.motor_value[0] = 0;
        control.motor_value[1] = 2;
        dshot_write(control.motor_value);
        os::sleep(260);
        control.motor_value[1] = 0;
        control.motor_value[2] = 3;
        dshot_write(control.motor_value);
        os::sleep(280);
        control.motor_value[2] = 0;
        control.motor_value[3] = 4;
        dshot_write(control.motor_value);
        os::sleep(280);
        control.motor_value[3] = 0;

        mainLastWakeTime = xTaskGetTickCount();
    }

    void rtLoop() {

        static uint32_t counter = 0;

        static crsf::Channels currentControlStatus{};

        auto& dataHolder = crsf::ELRSController::INSTANCE->controlData;
        {
            auto optional = dataHolder.get();
            if (optional.has_value()){
                currentControlStatus = optional.value();
            }
        }



        dev::pcb_led.toggle();

        if (sensor::bmpData.hasNew()) {

            auto r = sensor::bmpData.get().value(); // just make it shorter

            //sprintf(msg.get(), "pressure: %f, temp: %f\n", r.first, r.second);
            //log("pressure: " + std::to_string(r.first) + ", temp: " + std::to_string(r.second) + "\n");
            //HAL_UART_Transmit_DMA(&huart8, reinterpret_cast<const uint8_t *>(msg.c_str()), msg.size());
        }

        if (sensor::bmmData.hasNew()) {
            auto r = sensor::bmmData.get().value();

            //log("magnetic vector:\t" + std::to_string(r.x) + "\t" + std::to_string(r.y) + "\t" + std::to_string(r.z) + "\n");
        }

        {
            auto r = dev::bmi->getData();
            auto& a = r.first;
            auto& g = r.second;

            //log("accel:\t" + std::to_string(a.x) + "\t" + std::to_string(a.y) + "\t" + std::to_string(a.z) + "\n");
            //log("gyro:\t" + std::to_string(g.x) + "\t" + std::to_string(g.y) + "\t" + std::to_string(g.z) + "\n");
        }

        //log("main loop took " + std::to_string((xTaskGetTickCount() - mainLastWakeTime)/(float)frequency) + " time to run\n");
        log(std::format("CH1: {}\tCH2: {}\tCH3: {}\tCH4: {}\n", currentControlStatus[0], currentControlStatus[1], currentControlStatus[2], currentControlStatus[3]));

        counter++;
        if (counter >= 4) counter = 0;
        vTaskDelayUntil(&mainLastWakeTime, frequency);


        dev::pcb_led.setState(dshot::fromFloat(currentControlStatus[2]) <= 50);
        control.motor_value[0] = dshot::fromFloat(currentControlStatus[2]) - 2;
        dshot_write(control.motor_value);

        //vTaskDelay(frequency);
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

