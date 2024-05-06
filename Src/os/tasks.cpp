//
// Created by kosmx on 10/31/23.
//

#include "tasks.h"
#include "os.h"
#include "telemetry.h"
#include "usart.h"
#include "driver/dshot.h"

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
        CommStation::INSTANCE = p = new crsf::ELRSController(*os::uart_dma::getFor(&huart10, 64*64));
        p->initComm();

        dshot_init(dshot_type_e::DSHOT300);

        /**
        os::Task esc_logger("ESC_logger");
        esc_logger.start({
            auto& esc_uart = os::uar
            while(true) {

            }
        });*/
    }


}
