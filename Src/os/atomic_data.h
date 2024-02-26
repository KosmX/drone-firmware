//
// Created by kosmx on 11/11/23.
//

#ifndef STUFF_ATOMIC_DATA_H
#define STUFF_ATOMIC_DATA_H
#ifdef __cplusplus

#include "os.h"
#include "mutex_wapper.h"
#include <optional>

namespace os {

    template<typename T>
    class AtomicData {
    private:
        Mutex mutex{};
        bool newData = false;
        TickType_t lastUpdate{};
        T value{};
    public:
        inline void set(T val) {
            auto l = mutex.lock();

            lastUpdate = xTaskGetTickCount();
            newData = true;
            value = val;

            l.unlock();
        }

        /**
         * @return the last data, does not set or reset the newData flag
         */
        [[nodiscard]] inline T peek() {
            auto l = mutex.lock();
            return value;
        }

        [[nodiscard]] TickType_t lastUpdated() const {
            return lastUpdate; // should be atomic
        }

        [[nodiscard]] bool hasNew() const {
            return newData;
        }

        /**
         * Gets the new data or return null if no new data is available
         * @return
         */
        [[nodiscard]] std::optional<T> get() {
            if (newData) {
                auto l = mutex.lock();
                newData = false;
                return value;
            }
            return std::nullopt;
        }

        AtomicData& operator=(T t) {
            this->set(t);
            return *this;
        }
    };

}

#endif //__cplusplus
#endif //STUFF_ATOMIC_DATA_H
