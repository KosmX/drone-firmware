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
        if (locked) {
            mutex.unlock();
            locked = false;
        }
    }

    MutexLock &MutexLock::operator=(MutexLock&& other) noexcept {
        this->locked = other.locked;
        this->mutex = other.mutex;
        other.locked = false;
        return *this;
    }
} // os