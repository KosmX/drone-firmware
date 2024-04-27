//
// Created by kosmx on 10/31/23.
//

#include "tasks.h"
#include "os.h"
#include "telemetry.h"
#include "usart.h"

#include "timers.h"
#include "crsf/CommStation.h"

/**
 * Start tasks from RT thread, then return.
 * When this returns, the control loop can start immediately,
 */

// make sure we won't have any name conflict
namespace tasks {

    os::AtomicData<DShotData> motorData{};

    void init() {
        //tlm::ITelemetry::INSTANCE = new tlm::DummyTelemetry();  // For creating a /dev/null telemetry



        crsf::ELRSController *p;
        CommStation::INSTANCE = p = new crsf::ELRSController(*os::uart_dma::getFor(&huart10, 64*16));
        p->initComm();

        auto timer = xTimerCreate(
                "motor_setup_timer",
                pdMS_TO_TICKS(20),
                true,
                nullptr,
                [](TimerHandle_t timer) {

                });
        //xTimerStart(timer, 10);

    }


}
