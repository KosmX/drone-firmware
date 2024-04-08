//
// Created by Cynthia on 4/8/24.
//

#ifndef DRONE_FW_SEMAPHORE_H
#define DRONE_FW_SEMAPHORE_H
#include "os.h"

namespace os {

    class Semaphore {
    private:
        StaticSemaphore_t sData{};
        SemaphoreHandle_t handle;
    public:
        Semaphore(const Semaphore&) = delete;
        Semaphore& operator=(const Semaphore&) = delete;


        /**
         * Creates a new semaphore object
         * Newly created semaphore is taken by default.
         */
        Semaphore();
        ~Semaphore();

        void take();
        void give();
        bool takeOrPass();
        bool takeFromISR();


    };

} // os

#endif //DRONE_FW_SEMAPHORE_H
