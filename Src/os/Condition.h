//
// Created by kosmx on 4/29/24.
//

#ifndef DRONE_FW_CONDITION_H
#define DRONE_FW_CONDITION_H

#include <functional>
#include "os.h"
#include "mutex_wapper.h"

namespace os {

    class Condition {

    private:
        volatile TaskHandle_t taskToNotify = nullptr;
    public:
        Condition(const Condition&) = delete;
        Condition& operator=(const Condition&) = delete;

        bool wait(MutexLock&, uint32_t maxWait = portMAX_DELAY);

        void notify();

        /**
         * @warning ONLY USE THIS FROM INTERRUPT
         */
        void notifyFromISR();
    };

} // os

#endif //DRONE_FW_CONDITION_H
