//
// Created by kosmx on 5/4/24.
//

#ifndef DRONE_FW_CONTROL_H
#define DRONE_FW_CONTROL_H
#ifdef __cplusplus

// RT priority control loop
#include "MadgwickAHRS.h"
#include "crsf/Packets.h"
#include <cinttypes>

class Control {
private:
    Madgwick madgwick;
    crsf::Channels currentControlStatus{};
    uint32_t counter = 0;


public:
    uint16_t motor_value[4] = {0, 0, 0, 0};
    void controlLoop();
};


#endif
#endif //DRONE_FW_CONTROL_H
