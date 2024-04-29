//
// Created by kosmx on 10/18/23.
//

#ifndef STUFF_USER_MAIN_H
#define STUFF_USER_MAIN_H

#ifdef __cplusplus

#include <cstdint>

namespace entry {

/**
 * Real-time thread loop
 */
    void rtLoop();

    void init();
}
// C functions

extern "C" {
#else
#include "stdint.h"
#endif

    void preInit();

    void taskError();
    void user_assert_failed(uint8_t *file, uint32_t line);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif //STUFF_USER_MAIN_H
