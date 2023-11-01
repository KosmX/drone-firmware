//
// Created by kosmx on 10/31/23.
//

#include "tasks.h"
#include "os.h"
#include "telemetry.h"
#include "usart.h"

void init_tasks() {
    tlm::ITelemetry::INSTANCE = new tlm::UartTelemetry(huart8); // send telemetry to UART
    //tlm::ITelemetry::INSTANCE = new tlm::DummyTelemetry();  // For creating a /dev/null telemetry
}