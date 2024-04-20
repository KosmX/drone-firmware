//
// Created by kosmx on 4/4/24.
//

#include "Config.h"

namespace cfg {

    BoolConfig armed{"armed", false};
    BoolConfig disarmSafety{"disarmSafety", false};


    std::vector<ConfigEntry*> cfg{
        &armed,
        &disarmSafety,
    };
} // cfg