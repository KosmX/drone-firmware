//
// Created by kosmx on 11/11/23.
//

#include "devices.h"

namespace dev {

    os::led pcb_led{LED_D1_GPIO_Port, LED_D1_Pin};
    os::i2c* i2c1 = nullptr;
    drv::bmp* bmp = nullptr;

    void init() {
        i2c1 = os::i2c::getFor(&hi2c1);


        bmp = new drv::bmp(*i2c1, 0b1110110);
    }

}
