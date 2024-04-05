//
// Created by kosmx on 3/22/24.
//

#ifndef DRONE_FW_CONFIGRESPONSEBUILDER_H
#define DRONE_FW_CONFIGRESPONSEBUILDER_H
#include <memory>

namespace cfg {

    class ConfigResponseBuilder {
    public:
        ConfigResponseBuilder(const ConfigResponseBuilder&) = delete;
        void operator=(const ConfigResponseBuilder&) = delete;
    private:
        int idx = 0;
        size_t size = 0;
        std::unique_ptr<unsigned char> data;

        int configId;
        int parentId;

    public:
        ConfigResponseBuilder(int configId, int parentId, int packetPart = 0): configId(configId), parentId(parentId) {
            // TODO
        }

        bool writeByte()

    };

} // cfg

#endif //DRONE_FW_CONFIGRESPONSEBUILDER_H
