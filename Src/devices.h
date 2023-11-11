//
// Created by kosmx on 11/11/23.
//

#ifndef STUFF_DEVICES_H
#define STUFF_DEVICES_H
#ifdef __cplusplus

// includes
#include "i2c.h"

#include "os/led.h"
#include "os/os_i2c.h"
#include "driver/bmp.h"

namespace dev {

// direct comms (bus etc)

    extern os::led pcb_led;
    extern os::i2c* i2c1;

// devices

    extern drv::bmp* bmp;

// init function (call after OS is ready)
    void init();

}

#endif //__cplusplus
#endif //STUFF_DEVICES_H
