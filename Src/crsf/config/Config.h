//
// Created by kosmx on 4/4/24.
//

#ifndef DRONE_FW_CONFIG_H
#define DRONE_FW_CONFIG_H
#include "ConfigEntry.h"
#include <memory>
#include <vector>

namespace cfg {

    extern std::vector<std::unique_ptr<ConfigEntry>> cfg;

    
} // cfg

#endif //DRONE_FW_CONFIG_H
