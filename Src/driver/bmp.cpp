//
// Created by kosmx on 11/7/23.
//

#include "bmp.h"
#include "os/os.h"

namespace drv {

    bmp::bmp(os::i2c& i2c, uint8_t devaddr): dev{0, this, BMP3_I2C_INTF}, i2c{i2c}, addr{static_cast<uint8_t>(devaddr << 1)} {

        dev.delay_us = [](uint32_t period, void* _this){
            vTaskDelay(pdMS_TO_TICKS(period)); // no active waiting
        };

        dev.write = [](uint8_t reg_addr, const uint8_t* writeData, uint32_t len, void* pthis) -> BMP3_INTF_RET_TYPE {
            auto* _this = reinterpret_cast<bmp*>(pthis);

            _this->i2c.write(_this->addr, reg_addr, writeData, len, true);

            return 0;
        };
    }

}