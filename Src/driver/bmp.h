//
// Created by kosmx on 11/7/23.
//

#ifndef STUFF_BMP_H
#define STUFF_BMP_H

#ifdef __cplusplus
#include "bmp3.h"
#include "i2c.h"

namespace drv {

    class bmp {
    private:
        struct bmp3_dev dev;
        I2C_HandleTypeDef* hi2c;

        int8_t write(uint8_t reg_addr, const uint8_t *read_data, uint32_t len);

    public:
        bmp(I2C_HandleTypeDef* i2c);
    };
}

#endif

#endif //STUFF_BMP_H
