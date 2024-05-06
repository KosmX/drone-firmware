//
// Created by kosmx on 10/28/23.
//

#ifndef STUFF_OS_H
#define STUFF_OS_H

#include "FreeRTOS.h"
#include "semphr.h"

// generic OS related shortcuts
namespace os {
    void sleep(uint32_t ms);
}

#endif //STUFF_OS_H
