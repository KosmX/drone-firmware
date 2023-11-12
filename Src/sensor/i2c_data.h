//
// Created by kosmx on 11/11/23.
//

#ifndef STUFF_I2C_DATA_H
#define STUFF_I2C_DATA_H
#ifdef __cplusplus

#include "driver/bmp.h"
#include "driver/bmm.h"
#include "os/mutex_wapper.h"
#include "os/atomic_data.h"

namespace sensor {
    extern os::AtomicData<std::pair<float, float>> bmpData;
    extern os::AtomicData<bmm150_mag_data> bmmData;

    void i2c_init();

}

#endif //__cplusplus
#endif //STUFF_I2C_DATA_H
