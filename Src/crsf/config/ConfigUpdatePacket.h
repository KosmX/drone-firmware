//
// Created by kosmx on 3/22/24.
//

#ifndef DRONE_FW_CONFIGUPDATEPACKET_H
#define DRONE_FW_CONFIGUPDATEPACKET_H
#include <memory>
#include <vector>
#include "crsf/CRSF_listener.h"

namespace cfg {

    /**
     * crsf packet but with some extra functionality for generic config use
     */
    class ConfigUpdatePacket : public crsf::Packet {
    private:
    public:
        ConfigUpdatePacket(const crsf::Packet &packet) : crsf::Packet(packet) {
            assert_param(isExtended());
        }

        explicit ConfigUpdatePacket(const os::RingBufferEntryPtr& pData): crsf::Packet(pData) {}





    };

} // cfg

#endif //DRONE_FW_CONFIGUPDATEPACKET_H
