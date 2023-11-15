//
// Created by kosmx on 11/7/23.
//

#include "bmp.h"
#include "os/os.h"

namespace drv {

    bmp::bmp(os::i2c& i2c, uint8_t devaddr): dev{0, this, BMP3_I2C_INTF}, settings{}, i2c{i2c}, addr{static_cast<uint8_t>(devaddr << 1)} {


        dev.delay_us = [](uint32_t period, void* _this){
            vTaskDelay(pdMS_TO_TICKS((period + 999)/1000)); // no active waiting
        };

        dev.write = [](uint8_t reg_addr, const uint8_t* writeData, uint32_t len, void* pthis) -> BMP3_INTF_RET_TYPE {
            auto* _this = reinterpret_cast<bmp*>(pthis);

            _this->i2c.write(_this->addr, reg_addr, writeData, len);

            return BMP3_INTF_RET_SUCCESS;
        };

        dev.read = [](uint8_t reg_addr, uint8_t* readData, uint32_t len, void* pthis) -> BMP3_INTF_RET_TYPE {
            auto* _this = reinterpret_cast<bmp*>(pthis);

            _this->i2c.read(_this->addr, reg_addr, readData, len);

            return BMP3_INTF_RET_SUCCESS;
        };


        auto r = bmp3_init(&dev);
        if (r != BMP3_OK) {
            Error_Handler(); // for callstack
        }

        //settings.int_settings.drdy_en = BMP3_ENABLE;
        settings.press_en = BMP3_ENABLE;
        settings.temp_en = true;

        settings.int_settings.drdy_en = BMP3_ENABLE;
        settings.odr_filter.press_os = BMP3_OVERSAMPLING_8X;
        settings.odr_filter.temp_os = BMP3_NO_OVERSAMPLING;
        settings.odr_filter.iir_filter = BMP3_IIR_FILTER_COEFF_3;
        settings.odr_filter.odr = BMP3_ODR_50_HZ;
        uint16_t settings_sel;
        settings_sel = BMP3_SEL_PRESS_EN | BMP3_SEL_TEMP_EN | BMP3_SEL_PRESS_OS | BMP3_SEL_TEMP_OS | BMP3_SEL_ODR | BMP3_SEL_DRDY_EN;

        settings.op_mode = BMP3_MODE_NORMAL;

        r = bmp3_set_sensor_settings(settings_sel, &settings, &dev);
        if (r != BMP3_OK) {
            Error_Handler(); // for callstack
        }
        bmp3_set_op_mode(&settings, &dev);
    }

    std::optional<std::pair<float, float>> bmp::activateAndRead() {
        //bmp3_set_op_mode(&settings, &dev);
        struct bmp3_status status{};
        auto rslt = bmp3_get_status(&status, &dev);

        if (rslt == BMP3_OK && status.intr.drdy == BMP3_ENABLE) {

            bmp3_get_sensor_data(BMP3_PRESS_TEMP, &data, &dev);
            return std::pair(data.pressure, data.temperature);
        } else {
            return std::nullopt;
        }
    }

}