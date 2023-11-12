//
// Created by kosmx on 11/7/23.
//

#ifndef STUFF_BMM_H
#define STUFF_BMM_H
#ifdef __cplusplus

#include "os/os_i2c.h"
#include "bmm150.h"

namespace drv {

    class bmm {
    private:
        struct bmm150_dev dev;
        struct bmm150_settings settings;
        os::i2c& i2c;
        uint8_t addr;


    public:
        bmm(os::i2c& i2c, uint8_t devaddr);

        //std::optional<std::pair<float, float>> activateAndRead();

        void startMeasurement();

        struct bmm150_mag_data getData();
    };
}


#endif //__cplusplus
#endif //STUFF_BMM_H
