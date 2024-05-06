//
// Created by kosmx on 5/4/24.
//

#ifndef DRONE_FW_CONTROL_H
#define DRONE_FW_CONTROL_H
#ifdef __cplusplus

// RT priority control loop
#include "MadgwickAHRS.h"
#include <cinttypes>

class Control {
private:
    Madgwick madgwick;


public:
    uint16_t motor_value[4] = {0, 0, 0, 0};
    void controlLoop();
};


#endif
#endif //DRONE_FW_CONTROL_H
