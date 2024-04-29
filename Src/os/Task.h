//
// Created by kosmx on 4/29/24.
//

#ifndef DRONE_FW_TASK_H
#define DRONE_FW_TASK_H

#include "os.h"
#include "cmsis_os2.h"
#include <functional>

namespace os {

    /**
     * Create OS task with C++ lambdas
     */
    class Task {
    private:

        const osThreadAttr_t thread_attr;
        /*
         * {
                .name = "CRSF_listen",
                .stack_size = 128 * 4,
                .priority = (osPriority_t) osPriorityNormal,
        };
         */

        osThreadId_t threadId = nullptr;

    public:
        explicit Task(const char* threadName, osPriority_t taskPriority = osPriorityNormal, size_t stackSize = 128*4);

        // no copy
        Task(const Task&) = delete;
        Task& operator=(const Task&) = delete;

        void start(std::function<void()>&& entry);
    };

} // task

#endif //DRONE_FW_TASK_H
