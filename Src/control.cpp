//
// Created by kosmx on 5/4/24.
//

#include "control.h"
#include "devices.h"
#include "driver/dshot.h"
#include "crsf/Packets.h"
#include "crsf/CommStation.h"
#include "sensor/builtin_sensor_data.h"


void Control::controlLoop() {


    auto& dataHolder = CommStation::controlData;
    {
        auto optional = dataHolder.get();
        if (optional.has_value()){
            currentControlStatus = optional.value();
        }
    }


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

    dev::pcb_led.setState(dshot::fromFloat(currentControlStatus[2]) <= 50);
    motor_value[1] = dshot::fromFloat(currentControlStatus[2]) - 2;
    motor_value[2] = dshot::fromFloat(currentControlStatus[2]) - 2;
    motor_value[3] = dshot::fromFloat(currentControlStatus[2]) - 2;
    motor_value[0] = dshot::fromFloat(currentControlStatus[2]) - 2;
    dshot_write(motor_value);


    counter++;

    if (counter >= 4) counter = 0;

    //vTaskDelay(frequency);
}
