//
// Created by kosmx on 10/18/23.
//

#ifndef STUFF_USER_MAIN_H
#define STUFF_USER_MAIN_H

#ifdef __cplusplus

namespace entry {

/**
 * Real-time thread loop
 */
    void rtLoop();

    void init();
}
// C functions

extern "C" {
#endif

    void preInit();

#ifdef __cplusplus
}
#endif // __cplusplus

#endif //STUFF_USER_MAIN_H
