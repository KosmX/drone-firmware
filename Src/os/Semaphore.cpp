//
// Created by kosmx on 4/8/24.
//

#include "Semaphore.h"

namespace os {
    Semaphore::Semaphore(): handle(xSemaphoreCreateBinaryStatic(&sData)) {}

    Semaphore::~Semaphore() {
        vSemaphoreDelete(handle);
    }

    void Semaphore::give() {
        xSemaphoreGive(handle);
    }

    void Semaphore::take() {
        xSemaphoreTake(handle, portMAX_DELAY);
    }

    bool Semaphore::takeOrPass() {
        return xSemaphoreTake(handle, 0) != pdFALSE;
    }

    bool Semaphore::takeFromISR() {
        BaseType_t b = false;
        bool r = xSemaphoreGiveFromISR(handle, &b) != pdFALSE;
        portYIELD_FROM_ISR(b);
        return r;
    }


} // os