//
// Created by kosmx on 11/11/23.
//

#include "i2c_data.h"
#include "timers.h"
#include "devices.h"

namespace sensor {
    os::AtomicData<std::pair<float, float>> bmpData{};
    void i2c_init() {
        auto timer = xTimerCreate(
                "i2c1_read_timing",
                pdMS_TO_TICKS(10),
                true,
                nullptr,
                [](TimerHandle_t timer) {
                    auto r = dev::bmp->activateAndRead();
                    if (r.has_value()) {
                        bmpData = r.value();
                    }
                });
        xTimerStart(timer, 10);
    }
}
