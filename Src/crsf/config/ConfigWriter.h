//
// Created by kosmx on 3/22/24.
//

#ifndef DRONE_FW_CONFIGWRITER_H
#define DRONE_FW_CONFIGWRITER_H
#include <memory>
#include <vector>
#include "crsf/CRSF_listener.h"

namespace cfg {

    class ConfigWriter {
    private:
        crsf::Packet packet;
    public:
        ConfigWriter(crsf::Packet packet);

        void write();

    };

} // cfg

#endif //DRONE_FW_CONFIGWRITER_H
