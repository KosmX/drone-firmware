//
// Created by kosmx on 10/28/23.
//

#ifndef STUFF_MUTEX_WAPPER_H
#define STUFF_MUTEX_WAPPER_H

#include "os.h"

namespace os {

    class Mutex;

    /**
     * Control instance for mutex<br>
     * Every thread should use the same control instance (it is not thread safe)
     * <p>
     * When destroyed, it will unlock the associated lock
     *
     * @example
     *
     *     {
     *       auto l = mutex.lock(); // this will acquire the lock and create a control instance
     *
     *       l.unlock() // possible
     *       l.lock() // also possible
     *
     *     } // when leaving the block, the instance will be destroyed, the lock will be freed (if wasn't earlier)
     *
     */
    class MutexLock {
    private:
        Mutex& mutex;
        bool locked = true;
        MutexLock(const MutexLock&);
    public:
        explicit MutexLock(Mutex& m);

        void unlock();
        void lock();

        MutexLock& operator=(MutexLock&& other) noexcept;
        operator bool() const;

        ~MutexLock();
    };

    class Mutex {
    private:
        StaticSemaphore_t pxMutexBuffer{};
        SemaphoreHandle_t mutex;
    public:

        Mutex();

        void lockUnsafe();

        MutexLock lock();

        void unlock();
    };

} // os

#endif //STUFF_MUTEX_WAPPER_H
