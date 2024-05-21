//
// Created by kosmx on 3/16/24.
//

#ifndef DRONE_FW_UART_DMA_H
#define DRONE_FW_UART_DMA_H

#include "os/os.h"
#include "usart.h"
#include "mutex_wapper.h"

namespace os {

    // something not so fancy
    /**
     * Special pointer type with ring-buffer actions
     * It is a bit more like to Java iterator than C++ pointers.
     */
    class RingBufferEntryPtr {

        // The original buffer
        // must have a size of a power of 2
        const uint8_t* const buffer;

        // buffer size mask (size - 1)
        size_t sizeMask;
        size_t startPos;
        size_t pos;
#ifdef USE_FULL_ASSERT
        size_t dataSize;
#endif

    public:
        RingBufferEntryPtr(const uint8_t* buffer, size_t size, size_t pos, size_t data_size);
        RingBufferEntryPtr(const RingBufferEntryPtr& other) = default; // Default copy ctor


        uint8_t operator[](size_t pos) const;

        char operator*() const;
        RingBufferEntryPtr operator+(size_t increment) const;
        RingBufferEntryPtr operator-(size_t increment) const;
        RingBufferEntryPtr& operator++(); // pre increment
        RingBufferEntryPtr operator++(int); // post increment
        RingBufferEntryPtr& operator--(); // pre increment
        RingBufferEntryPtr operator--(int); // post increment

        // now we can do *p++

        // Just to show it's here.
        bool operator==(const RingBufferEntryPtr& other) const = default; // Bitwise compare is enough for this class

    };


    class uart_dma {
    private:
        UART_HandleTypeDef * huart;
        //SemaphoreHandle_t readSemaphore;
        SemaphoreHandle_t writeSemaphore;

        // UART can do TX without any RX synchronization
        mutable Mutex readMutex; // mutable: allow synchronized operation from const methods
        mutable Mutex writeMutex;

        uint8_t* const recBuffer;
        const size_t recBufferLen;
        size_t recBufferPos = 0;

        [[nodiscard]] inline size_t getDptr() const;

        uart_dma(UART_HandleTypeDef* huart, size_t bufSize);

        [[nodiscard]] size_t _available() const;
    public:
        static uart_dma* getFor(UART_HandleTypeDef * uart, size_t bufferSize);

        /* -------- Read specific functions -------- */
        [[nodiscard]] size_t available() const;

        /**
         * Peek into the buffer without reading
         * @param next peek index (needed to peek the size byte when using for CRSF)
         * @return next byte
         */
        [[nodiscard]] uint8_t peek(size_t next = 0) const;

        /**
         * Return a pointer to the available data and move buffer ptr by amount.
         * The data pointed by the ptr will be overwritten after some time, if the data is needed, please copy it.
         * @param amount data size to get
         * @return pointer to data inside the buffer
         */
        RingBufferEntryPtr read(size_t amount);  // *(uart->read(1));   c:

        /**
         * Reset RX buffer
         */
        void clear();

        /* -------- Write specific functions -------- */
        
        HAL_StatusTypeDef write(const uint8_t* data, size_t len);

        ~uart_dma(); // Not going to be used, but it doesn't hurt

        void stop();
    };

} // os

#endif //DRONE_FW_UART_DMA_H
