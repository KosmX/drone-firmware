//
// Created by kosmx on 4/29/24.
//

#include "Condition.h"
#include <limits>
#include "stm32f4xx_hal.h"

namespace os {
    bool Condition::wait(MutexLock& lock, uint32_t maxWait) {
        assert_param(lock); // lock must be taken when invoking wait
        if (taskToNotify) {
            assert_param(false); // this shouldn't happen
        }
        taskToNotify = xTaskGetCurrentTaskHandle();
        bool r = xTaskNotifyWait(pdFALSE, std::numeric_limits<uint32_t>::max(), nullptr, portMAX_DELAY) == pdPASS;
        lock.lock();
        return r;
    }

    void Condition::notifyFromISR() {
        TaskHandle_t handle = this->taskToNotify;
        if (handle) {
            BaseType_t f = pdFALSE;
            xTaskNotifyFromISR(handle, 0, eNoAction, &f);
            portYIELD_FROM_ISR(f);
        }
    }

    void Condition::notify() {
        TaskHandle_t handle = this->taskToNotify;
        if (handle) {
            xTaskNotify(handle, 0, eNoAction);
        }
    }
} // os