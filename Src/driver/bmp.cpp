//
// Created by kosmx on 11/7/23.
//

#include "bmp.h"
#include "os/os.h"

namespace drv {

    bmp::bmp(I2C_HandleTypeDef *i2c): dev{0, this, BMP3_I2C_INTF}, hi2c{i2c} {

        dev.delay_us = [](uint32_t period, void* _this){
            vTaskDelay(pdMS_TO_TICKS(period)); // no active waiting
        };

        dev.write = [](uint8_t reg_addr, const uint8_t* read_data, uint32_t len, void* pthis) -> BMP3_INTF_RET_TYPE {
            return reinterpret_cast<bmp*>(pthis)->write(reg_addr, read_data, len);
        };
    }

    int8_t bmp::write(uint8_t reg_addr, const uint8_t *read_data, uint32_t len) {

        return 0;
    }
}