//
// Created by kosmx on 11/7/23.
//

#ifndef STUFF_OS_I2C_H
#define STUFF_OS_I2C_H
#ifdef __cplusplus

#include "os/os.h"
#include "i2c.h"
#include "mutex_wapper.h"

namespace os {

    /**
     * I2C wrapper for proper IO methods
     * > I2CWrite will busy-wait, avoid that if possible
     * I2C DMA will only call-back, capturing that is hard
     *
     * This uses interrupts to start a transmission and suspend the callee thread until transmission is finished.
     */
    class os_i2c {
    private:
        I2C_HandleTypeDef* hi2c;
        SemaphoreHandle_t readSemaphore;
        SemaphoreHandle_t writeSemaphore;
        Mutex mutex;

        explicit os_i2c(I2C_HandleTypeDef* i2c);

        os_i2c(os_i2c&): hi2c{}, readSemaphore{}, writeSemaphore{} {}; // disable
        os_i2c& operator=(const os_i2c&);
    public:
        static os_i2c* getFor(I2C_HandleTypeDef* i2c);

        /**
         * Read from this I2C, current thread will be suspended until result.
         * @param dev_addr device address shifted left by 1
         * @param reg_addr memory address
         * @param read_data data buffer
         * @param len data size
         * @return status, if HAL_OK, read_data is now containing new data
         */
        HAL_StatusTypeDef read(uint8_t dev_addr, uint8_t reg_addr, uint8_t *read_data, uint32_t len);

        /**
         * Write into I2C device
         * @param dev_addr device address shifted left by 1
         * @param reg_addr mem addr
         * @param write_data data to write
         * @param len amount of data to write
         * @param suspend suspend thread until transmission is finished
         * @return status
         */
        HAL_StatusTypeDef write(uint8_t dev_addr, uint8_t reg_addr, const uint8_t *write_data, uint32_t len, bool suspend = false);
    };

} // os


#endif //__cplusplus
#endif //STUFF_OS_I2C_H
