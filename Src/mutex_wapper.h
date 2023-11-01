//
// Created by kosmx on 10/28/23.
//

#ifndef STUFF_MUTEX_WAPPER_H
#define STUFF_MUTEX_WAPPER_H

#include "os.h"

namespace os {

    class Mutex;

    class MutexLock {
    private:
        Mutex& mutex;
    public:
        explicit MutexLock(Mutex& m);

        ~MutexLock();
    };

    class Mutex {
    private:
        StaticSemaphore_t pxMutexBuffer{};
        SemaphoreHandle_t mutex;
    public:

        Mutex();

        MutexLock lock();

        void unlock();
    };

} // os

#endif //STUFF_MUTEX_WAPPER_H
