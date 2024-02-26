//
// Created by kosmx on 11/11/23.
//

#include "builtin_sensor_data.h"
#include "timers.h"
#include "devices.h"

namespace sensor {
    os::AtomicData<std::pair<float, float>> bmpData{};
    os::AtomicData<bmm150_mag_data> bmmData{};

    void sensors_init() {
        auto timer = xTimerCreate(
                "i2c1_read_timing",
                pdMS_TO_TICKS(20),
                true,
                nullptr,
                [](TimerHandle_t timer) {

                    // BMM150
                    {
                        dev::bmm->startMeasurement();
                        auto r = dev::bmm->getData();
                        bmmData = r;
                    }

                    // BMP390
                    {
                        auto r = dev::bmp->activateAndRead();
                        if (r.has_value()) {
                            bmpData = r.value();
                        }
                    }
                });
        xTimerStart(timer, 10);
    }
}
