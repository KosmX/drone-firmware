//
// Created by kosmx on 5/5/24.
//
#include "os.h"

namespace os {
    void sleep(uint32_t ms) {
        vTaskDelay(ms/portTICK_PERIOD_MS);
    }
}