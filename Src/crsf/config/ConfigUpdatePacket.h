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
    class ConfigUpdatePacket : public crsf::RxPacket {
    public:
        // actually implicit
        ConfigUpdatePacket(const crsf::RxPacket &packet) : crsf::RxPacket(packet) {
            assert_param(isExtended());
        }

        explicit ConfigUpdatePacket(const os::RingBufferEntryPtr& pData): crsf::RxPacket(pData) {}

        [[nodiscard]] uint8_t getFieldIndex() const;

        [[nodiscard]] os::RingBufferEntryPtr data();




    };

} // cfg

#endif //DRONE_FW_CONFIGUPDATEPACKET_H
