//
// Created by kosmx on 10/22/23.
//

#include "telemetry.h"

#include "cmsis_os2.h"

extern "C" void loggingTask(void* arg) {
    while (true) {
        osDelay(1000);
    }
}
