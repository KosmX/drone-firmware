//
// Created by kosmx on 11/7/23.
//

#include "os_i2c.h"
#include <utility>
#include <vector>
#include <algorithm>

namespace os {

    std::vector<std::pair<I2C_HandleTypeDef*, i2c*>> i2c_instances{};

    i2c::i2c(I2C_HandleTypeDef *i2c) :
            hi2c{i2c},
            readSemaphore{xSemaphoreCreateBinary()}, writeSemaphore{xSemaphoreCreateBinary()},
            mutex() {

        //xSemaphoreGive(readSemaphore); // taken by default
        //xSemaphoreGive(writeSemaphore);

        if( std::any_of(i2c_instances.cbegin(), i2c_instances.cend(), [=](auto& other){return other.first == i2c;})) {
            // error?
            throw "Multiple inits for the same singleton";
        }

        // TODO convert this to a memory function generator with captures
        hi2c->MemRxCpltCallback = ([](I2C_HandleTypeDef *hi2c){
            auto* _this = i2c::getFor(hi2c);
            auto pTrue = pdTRUE;
            xSemaphoreGiveFromISR(_this->readSemaphore, &pTrue);
        });

        hi2c->MemTxCpltCallback = [](I2C_HandleTypeDef* hi2c) {
            auto* _this = i2c::getFor(hi2c);
            auto pTrue = pdTRUE;
            xSemaphoreGiveFromISR(_this->writeSemaphore, &pTrue);
        };

    }

    HAL_StatusTypeDef i2c::read(uint8_t dev_addr, uint8_t reg_addr, uint8_t *read_data, uint32_t len) {
        auto lock = mutex.lock(); // only one device can use I2C
        //xSemaphoreTake(_this->wr)

        // i don't care if the semaphore is taken or not, but I need it to be taken.
        xSemaphoreTake(readSemaphore, 0);

        // start receiving
        auto r = HAL_I2C_Mem_Read_DMA(hi2c, dev_addr, reg_addr, I2C_MEMADD_SIZE_8BIT, read_data, len);
        if (r != HAL_OK) return r;

        // Block this thread until transmission is finished
        xSemaphoreTake(readSemaphore, portMAX_DELAY);

        return HAL_OK;
    }

    HAL_StatusTypeDef i2c::write(uint8_t dev_addr, uint8_t reg_addr, const uint8_t *write_data, uint32_t len) {
        auto lock = mutex.lock();

        xSemaphoreTake(writeSemaphore, 0);

        auto r = HAL_I2C_Mem_Write_DMA(hi2c, dev_addr, reg_addr, I2C_MEMADD_SIZE_8BIT,
                                       const_cast<uint8_t *>(write_data), len);
        if (r != HAL_OK) return r;

        xSemaphoreTake(writeSemaphore, portMAX_DELAY);
        return HAL_OK;
    }


    // This is only thread-safe if no-one is creating new
    i2c* i2c::getFor(I2C_HandleTypeDef *i2c) {

        auto entry = std::find_if(i2c_instances.cbegin(), i2c_instances.cend(), [=](auto& it) -> bool{
            return it.first == i2c;
        });
        if (entry != i2c_instances.cend()) {
            return entry->second;
        }
        i2c_instances.push_back(std::pair(i2c, new class i2c(i2c)));
        return (i2c_instances[i2c_instances.size() - 1]).second;
    }

} // os