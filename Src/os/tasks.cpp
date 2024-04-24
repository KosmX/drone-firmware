//
// Created by kosmx on 10/31/23.
//

#include "tasks.h"
#include "os.h"
#include "telemetry.h"
#include "usart.h"

#include "timers.h"

/**
 * Start tasks from RT thread, then return.
 * When this returns, the control loop can start immediately,
 */

// make sure we won't have any name conflict
namespace tasks {

    os::AtomicData<DShotData> motorData{};

    void init() {
        //tlm::ITelemetry::INSTANCE = new tlm::DummyTelemetry();  // For creating a /dev/null telemetry

        auto timer = xTimerCreate(
                "i2c1_read_timing",
                pdMS_TO_TICKS(20),
                true,
                nullptr,
                [](TimerHandle_t timer) {
                    // TODO find out how does the timing works
                });
        //xTimerStart(timer, 10);

    }


}
