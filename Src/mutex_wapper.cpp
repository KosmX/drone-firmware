//
// Created by kosmx on 10/28/23.
//


#include "cmsis_os2.h"
#include "mutex_wapper.h"
#include "semphr.h"

namespace os {
    Mutex::Mutex(): mutex(xSemaphoreCreateMutexStatic(&pxMutexBuffer)) {}

    MutexLock Mutex::lock() {
        xSemaphoreTake(mutex, portMAX_DELAY);
        return MutexLock(*this);
    }

    void Mutex::unlock() {
        xSemaphoreGive(mutex);
    }

    MutexLock::MutexLock(Mutex &m): mutex(m) {}

    MutexLock::~MutexLock() {
        mutex.unlock();
    }
} // os