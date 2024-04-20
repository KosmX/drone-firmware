//
// Created by kosmx on 3/22/24.
//

#include "ConfigResponseBuilder.h"
#include "Config.h"
#include "crsf/crsf_protocol.h"
#include <cmath>

namespace cfg {
    ConfigResponseBuilder ConfigResponseBuilder::create(crsf::TxPacket& pData, int configId, int parentId, int packetPart) {
        ConfigResponseBuilder crb{pData, configId, configId, configId};

        auto& config = *cfg::cfg[configId];

        crb.data.setPacketType(crsf_frame_type_e::CRSF_FRAMETYPE_PARAMETER_SETTINGS_ENTRY);
        crb.data.setExtDest(CRSF_ADDRESS_RADIO_TRANSMITTER);
        crb.data.setExtSrc(CRSF_ADDRESS_CRSF_TRANSMITTER);

        {
            crb.data.data()[0] = (configId);
            crb.data.data()[0] = crb.writeByte(0);
            crb.data.data()[0] = crb.writeByte(parentId);

            crb.writeByte(config.getTypeID());
            crb.writeByte(config.isHidden());

            auto& name = config.getName();
            for (auto& c : name) {
                crb.writeByte(c);
            }
            crb.writeByte(0); // null terminated string

            config.read(crb);
        }

        crb.data.calcCRCAndSetPacketLength(std::max(maxSize, crb.writePtr));


        return crb;
    }

    bool ConfigResponseBuilder::writeByte(uint8_t byte) {

        if (writePtr >= 0) {
            data.data()[writePtr] = byte;
        }

        writePtr++;
        return writePtr > maxSize;
    }
} // cfg