//
// Created by kosmx on 3/16/24.
//

#include "uart_dma.h"
#include <vector>
#include <cassert>
#include <algorithm>
#include <stdexcept>

namespace os {

    std::vector<std::pair<UART_HandleTypeDef *, uart_dma*>> uart_dma_instances{};


    uart_dma *uart_dma::getFor(UART_HandleTypeDef *uart, size_t bufferSize) { auto entry = std::find_if(uart_dma_instances.cbegin(), uart_dma_instances.cend(), [=](auto& it) -> bool{
            return it.first == uart;
        });
        if (entry != uart_dma_instances.cend()) {
            return entry->second;
        }
        uart_dma_instances.push_back(std::pair(uart, new uart_dma(uart, bufferSize)));
        return (uart_dma_instances[uart_dma_instances.size() - 1]).second;
    }

    uart_dma::uart_dma(UART_HandleTypeDef *huart, size_t bufSize):
        huart{huart},
        //readSemaphore{xSemaphoreCreateBinary()},
        writeSemaphore{xSemaphoreCreateBinary()},
        readMutex(),
        writeMutex(),
        recBuffer{new uint8_t [bufSize]},
        recBufferLen{bufSize} {

        assert(__builtin_popcount(bufSize) == 1);

        HAL_UART_RegisterCallback(huart, HAL_UART_CallbackIDTypeDef::HAL_UART_TX_COMPLETE_CB_ID, [](UART_HandleTypeDef* huart) {
            auto newTask = pdFALSE;
            xSemaphoreGiveFromISR(uart_dma::getFor(huart, 0)->writeSemaphore, &newTask);
            portYIELD_FROM_ISR(newTask)
        });

        HAL_UART_Receive_DMA(huart, recBuffer, bufSize);

        //xSemaphoreGive(readSemaphore);
    }

    uart_dma::~uart_dma() {
        delete[] recBuffer;
    }

    size_t uart_dma::getDptr() const {
        return recBufferLen - huart->hdmarx->Instance->NDTR;
    }

    size_t uart_dma::_available() const {
        int s = getDptr() - (int)recBufferPos;
        if (s < 0) s += recBufferLen;
        return s;
    }

    uint8_t uart_dma::peek(size_t next) const {
        auto m = readMutex.lock();
        if (next >= _available()) {
            throw std::out_of_range("No data to peek");
        }
        return recBuffer[(recBufferPos + next) & (recBufferLen - 1)];
    }

    RingBufferEntryPtr uart_dma::read(size_t amount) {
        auto m = readMutex.lock();
        if (amount > _available()) throw std::out_of_range("Not enough data");
        size_t tmpPos = recBufferPos;
        recBufferPos = (recBufferPos + amount) & (recBufferLen - 1);
        return {recBuffer, recBufferLen, tmpPos, amount};
    }

    size_t uart_dma::available() const {
        auto m = readMutex.lock();
        return _available();
    }

    HAL_StatusTypeDef uart_dma::write(const uint8_t* data, size_t len) {
        auto l = writeMutex.lock();

        // i don't care if the semaphore is taken or not, but I need it to be taken.
        //xSemaphoreTake(writeSemaphore, 0);

        HAL_UART_Transmit_DMA(huart, data, len);

        xSemaphoreTake(writeSemaphore, portMAX_DELAY);

        return HAL_OK;
    }

    void uart_dma::clear() {
        stop();
        HAL_UART_Receive_DMA(huart, recBuffer, recBufferLen);
    }

    void uart_dma::stop() {
        HAL_UART_DMAStop(huart);
    }

    RingBufferEntryPtr::RingBufferEntryPtr(const uint8_t *buffer, size_t size, size_t pos, size_t data_size):
    buffer{buffer},
    sizeMask{size - 1},
    pos{pos},
    startPos{pos}
#ifdef USE_FULL_ASSERT
    ,dataSize{data_size}
#endif
    {}

    char RingBufferEntryPtr::operator*() const {
#ifdef USE_FULL_ASSERT
        if (pos - startPos >= dataSize) {
            throw std::out_of_range{"ringbuffer index"};
        }
#endif

        return buffer[pos & sizeMask];
    }

    uint8_t RingBufferEntryPtr::operator[](size_t pos) const {
#ifdef USE_FULL_ASSERT
        if (this->pos - startPos + pos >= dataSize) {
            throw std::out_of_range{"ringbuffer index"};
        }
#endif
        return buffer[(this->pos + pos) & sizeMask];

    }

    RingBufferEntryPtr &RingBufferEntryPtr::operator++() {
        pos++;
        return *this;
    }

    RingBufferEntryPtr RingBufferEntryPtr::operator++(int) {
        RingBufferEntryPtr tmp = *this;
        pos++;
        return tmp;
    }

    RingBufferEntryPtr &RingBufferEntryPtr::operator--() {
        pos--;
        return *this;
    }

    RingBufferEntryPtr RingBufferEntryPtr::operator--(int) {
        RingBufferEntryPtr tmp = *this;
        pos--;
        return tmp;
    }

    RingBufferEntryPtr RingBufferEntryPtr::operator+(size_t increment) const {
        RingBufferEntryPtr p(*this);
        p.pos += increment;
        return p;
    }

    RingBufferEntryPtr RingBufferEntryPtr::operator-(size_t increment) const {
        return *this + -increment; //
    }

} // os