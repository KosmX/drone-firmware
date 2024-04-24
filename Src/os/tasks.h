//
// Created by kosmx on 10/31/23.
//

#ifndef STUFF_TASKS_H
#define STUFF_TASKS_H

#ifdef __cplusplus

#include <os/atomic_data.h>

namespace tasks {

    struct DShotData{
        uint16_t d[4];
    };



    extern os::AtomicData<DShotData> motorData;

void init();

}
#endif

#endif //STUFF_TASKS_H
