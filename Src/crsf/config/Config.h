//
// Created by kosmx on 4/4/24.
//

#ifndef DRONE_FW_CONFIG_H
#define DRONE_FW_CONFIG_H
#include "ConfigEntry.h"
#include <memory>
#include <vector>

/**
 * Config entry definitions
 */

namespace cfg {

    extern BoolConfig armed;

    // must be set to true when disarming in-flight
    extern BoolConfig disarmSafety;

    extern std::vector<ConfigEntry*> cfg;

    
} // cfg

#endif //DRONE_FW_CONFIG_H
