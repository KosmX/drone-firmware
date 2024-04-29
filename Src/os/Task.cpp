//
// Created by kosmx on 4/29/24.
//

#include "Task.h"

#include <utility>
#include "telemetry.h"

namespace os {
    Task::Task(const char *threadName, osPriority_t taskPriority, size_t stackSize): thread_attr{
            .name = threadName,
            .stack_size = stackSize,
            .priority = (osPriority_t) taskPriority,
    } {}

    // some name mangling for entry function
    extern "C" void $$task$entry_function(void* task) {
        auto* f = reinterpret_cast<std::function<void()>*>(task);
        try {
            f->operator()();
        } catch(std::exception& e) {
            log(e.what() + std::string("\n"));
        } catch (...) {
            log("Caught unknown exception\n");
        }
        delete f;
    }

    void Task::start(std::function<void()>&& entry) {
        auto* entryFunction = new std::function<void()>(std::move(entry));

        threadId = osThreadNew($$task$entry_function, entryFunction, &thread_attr);
    }
} // task