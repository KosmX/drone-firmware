//
// Created by kosmx on 3/22/24.
//

#include "ConfigUpdatePacket.h"

namespace cfg {
    uint8_t ConfigUpdatePacket::getFieldIndex() const {
        return RxPacket::data()[0];
    }

    os::RingBufferEntryPtr ConfigUpdatePacket::data() {
        return RxPacket::data() + 1;
    }
} // cfg