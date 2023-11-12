//
// Created by kosmx on 11/7/23.
//

#include "bmm.h"
#include "os/os.h"

namespace drv {

    bmm::bmm(os::i2c &i2c, uint8_t devaddr) : dev{0, BMM150_I2C_INTF, this}, settings{}, i2c{i2c},
                                                   addr{static_cast<uint8_t>(devaddr << 1)} {
        dev.delay_us = [](uint32_t period, void *_this) {
            vTaskDelay(pdMS_TO_TICKS(period)); // no active waiting
        };

        dev.write = [](uint8_t reg_addr, const uint8_t *writeData, uint32_t len, void *pthis) -> BMM150_INTF_RET_TYPE {
            auto *_this = reinterpret_cast<bmm *>(pthis);

            _this->i2c.write(_this->addr, reg_addr, writeData, len);

            return BMM150_INTF_RET_SUCCESS;
        };

        dev.read = [](uint8_t reg_addr, uint8_t *readData, uint32_t len, void *pthis) -> BMM150_INTF_RET_TYPE {
            auto *_this = reinterpret_cast<bmm *>(pthis);

            _this->i2c.read(_this->addr, reg_addr, readData, len);

            return BMM150_INTF_RET_SUCCESS;
        };


        auto r = bmm150_init(&dev);
        if (r != BMM150_OK) {
            Error_Handler();
        }

        settings.pwr_mode = BMM150_POWERMODE_SLEEP; // sensor will be used in forced mode to reach 50 hz odr

        settings.preset_mode = BMM150_PRESETMODE_ENHANCED; //max ODR in forced mode: 60, targeting 50
        bmm150_set_presetmode(&settings, &dev);

        bmm150_set_op_mode(&settings, &dev); // initial meas
    }

    void bmm::startMeasurement() {
        bmm150_set_op_mode(&settings, &dev);
    }

    struct bmm150_mag_data bmm::getData() {
        struct bmm150_mag_data data{};
        bmm150_read_mag_data(&data, &dev);
        return data;
    }

}