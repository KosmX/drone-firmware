//
// Created by kosmx on 11/7/23.
//

#ifndef STUFF_BMP_H
#define STUFF_BMP_H

#ifdef __cplusplus
#include "bmp3.h"
#include "i2c.h"
#include "os/os_i2c.h"

namespace drv {

    class bmp {
    private:
        struct bmp3_dev dev;
        os::i2c& i2c;
        uint8_t addr;

    public:
        bmp(os::i2c& i2c, uint8_t devaddr);
    };
}

#endif

#endif //STUFF_BMP_H
