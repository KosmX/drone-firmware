//
// Created by kosmx on 3/22/24.
//

#ifndef DRONE_FW_CONFIGRESPONSEBUILDER_H
#define DRONE_FW_CONFIGRESPONSEBUILDER_H
#include <memory>
 #include "crsf/CRSF_listener.h"

namespace cfg {

    class ConfigResponseBuilder {
    public:
        ConfigResponseBuilder(const ConfigResponseBuilder&) = delete;
        void operator=(const ConfigResponseBuilder&) = delete;

        // copy is disabled, however move is still possible
        ConfigResponseBuilder(ConfigResponseBuilder&&) = default;
        ConfigResponseBuilder& operator=(ConfigResponseBuilder&&) = default;

        static ConfigResponseBuilder create(crsf::TxPacket& packet, int configId, int parentId, int packetPart = 0);
    private:
        int idx = 0;
        size_t size = 0;
        crsf::TxPacket& data;

        int configId;
        int parentId;
        static constexpr int maxSize = 64-6-3; // 64 bit max for a message, 6 bits (with extended) for extra data
        int writePtr = 0;

    public:
        ConfigResponseBuilder(crsf::TxPacket& pData, int configId, int parentId, int packetPart = 0): data(pData), configId(configId), parentId(parentId) {
            writePtr = -packetPart * maxSize;
        }

        bool writeByte(uint8_t byte);

    };

} // cfg

#endif //DRONE_FW_CONFIGRESPONSEBUILDER_H
