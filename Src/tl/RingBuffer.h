//
// Created by kosmx on 4/8/24.
//

#ifndef DRONE_FW_RINGBUFFER_H
#define DRONE_FW_RINGBUFFER_H
#include <memory>
#include <optional>
#include "os/mutex_wapper.h"
#include "os/os.h"
#include <functional>
#include "os/Condition.h"

namespace tl {

    /**
     * Ring buffer for queues
     *
     * The buffer always allocate all required storage when initialized, memory alloc errors can't happen runtime if T doesn't allocate memory.
     *
     * @tparam T type
     * @tparam Allocator allocator for type
     */
    template<typename T, class Allocator = std::allocator<T>>
    class RingBuffer {
    private:
        Allocator alloc;
        T* pData = nullptr;
        size_t size;
        size_t readIdx = 0;
        size_t writeIdx = 0;
        size_t actualSize = 0;
        mutable os::Mutex sync{};
        mutable os::Condition popNotifier{};

    public:
        explicit RingBuffer(size_t size, Allocator allocator = Allocator()): alloc(allocator), size(size) {
            static_assert(std::is_same_v<T, typename decltype(alloc)::value_type>);
            pData = alloc.allocate(size);
        }

        RingBuffer(const RingBuffer& other): alloc(other.alloc), size(other.size), readIdx(other.readIdx), writeIdx(other.writeIdx) {
            auto l = other.sync.lock(); // lock other obj
            pData = alloc.allocate(size);
            for (size_t i = readIdx; i != writeIdx; i = (i + 1)%size) {
                const T& o = *other[i];
                new(&pData[i]) T(o);
            }
        };

        ~RingBuffer() {
            for (size_t i = readIdx; i != writeIdx; i = (i + 1)%size) {
                pData[i].~T();
            }
            alloc.deallocate(pData, size);
        }


        [[nodiscard]] bool isEmpty() const noexcept {
            return actualSize == 0;
        }

        [[nodiscard]] bool isNotEmpty() const noexcept {
            return !isEmpty();
        }

        [[nodiscard]] bool isFull() const noexcept {
            return actualSize == size;
        }
        T* push(const T& t) {
            auto l = sync.lock();
            if (isFull()) throw std::out_of_range("No more space in ringbuffer, size is already at " + std::to_string(size));
            T* data = &pData[writeIdx];

            writeIdx = writeIdx + 1;
            if (writeIdx == size) writeIdx = 0;
            actualSize++;

            new(data) T(t); // copy ctor

            popNotifier.notify();
            return data;
        }

        template<typename... Args>
        T* emplace(Args&&... args) {
            auto l = sync.lock();
            if (isFull()) throw std::out_of_range("No more space in ringbuffer, size is already at " + std::to_string(size));
            T* data = &pData[writeIdx];

            writeIdx = writeIdx + 1;
            if (writeIdx == size) writeIdx = 0;
            actualSize++;

            popNotifier.notify();
            return new(data) T(std::forward<Args>(args)...);
        }


        /**
         * 1. Put a new object into buffer
         * 2. initialize it with func
         * 3. notify waiting
         * @param func
         */
        void configBack(std::function<void(T&)> func) {
            auto l = sync.lock();
            if (isFull()) throw std::out_of_range("No more space in ringbuffer, size is already at " + std::to_string(size));
            T* data = &pData[writeIdx];

            writeIdx = writeIdx + 1;
            if (writeIdx == size) writeIdx = 0;
            actualSize++;


            new(data) T();
            func(*data);
            popNotifier.notify();
        }

        /**
         * Pop the first element. It will delete that entry and return a copy.
         * Use peek and pop_v instead
         * @return
         */
        std::optional<T> pop_t() {
            auto l = sync.lock();
            if (isEmpty()) {
                return {};
            } else {
                std::optional<T> v = pData[readIdx];
                pData[readIdx].~T();
                readIdx++;
                if (readIdx == size) readIdx = 0;
                actualSize--;

                return v;
            }
        }

        /**
         *
         * @return was anything deleted?
         */
        bool pop_v() {
            auto l = sync.lock();
            if (isEmpty()) {
                return false;
            } else {
                pData[readIdx].~T();

                readIdx++;
                if (readIdx == size) readIdx = 0;
                actualSize--;

                return true;
            }
        }

        [[nodiscard]] T& peek() {
            if (isEmpty()) {
                throw std::out_of_range("Ring buffer is empty, can't peek");
            }
            return pData[readIdx];
        }

        [[nodiscard]] const T& peek() const {
            if (isEmpty()) {
                throw std::out_of_range("Ring buffer is empty, can't peek");
            }
            return pData[readIdx];
        }

        /**
         * Wait for a new element, returns immediately if not empty
         */
        void wait() {
            // 1. return if not empty
            //if (isNotEmpty()) return;

            // 2. If it was empty, make sure it is still empty (lock)
            auto l = sync.lock();
            if (isNotEmpty()) return;

            popNotifier.wait(l);
        }

    };

} // tl

#endif //DRONE_FW_RINGBUFFER_H
