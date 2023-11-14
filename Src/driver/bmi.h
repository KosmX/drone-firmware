//
// Created by kosmx on 11/14/23.
//

#ifndef STUFF_BMI_H
#define STUFF_BMI_H
#ifdef __cplusplus

#include "os/os_spi.h"
#include "bmi088_mm.h"

namespace drv {

    class bmi {
    private:
        struct bmi08_dev dev;
        struct bmi08_data_sync_cfg syncCfg;

        os::spi& spi;
        os::gpin nGyro;
        os::gpin nAccel;

    public:
        bmi(os::spi& spi, os::gpin& nGyro, os::gpin& nAccel);
    };

} // drv

#endif //__cplusplus
#endif //STUFF_BMI_H
