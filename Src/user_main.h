//
// Created by kosmx on 10/18/23.
//

#ifndef STUFF_USER_MAIN_H
#define STUFF_USER_MAIN_H

#ifdef __cplusplus


/**
 * Real-time thread loop
 */
void rtLoop();

// C functions

extern "C" {
#endif

    void preInit();

    void init();


#ifdef __cplusplus
}
#endif // __cplusplus

#endif //STUFF_USER_MAIN_H
