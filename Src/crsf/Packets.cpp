//
// Created by kosmx on 4/22/24.
//

#include "Packets.h"

namespace crsf {
    void HeartBeat::write(TxPacket& p) {
        p.setPacketType(CRSF_FRAMETYPE_HEARTBEAT);
        p.writeByte(0);
        p.writeByte(CRSF_ADDRESS_FLIGHT_CONTROLLER);
    }

    ChannelData::ChannelData(const RxPacket &p): RxPacket(p) {
    }

    /**
     * https://github.com/crsf-wg/crsf/wiki/CRSF_FRAMETYPE_RC_CHANNELS_PACKED
     * @return
     */
    Channels ChannelData::readChannels() const {
        const unsigned srcBits = 11;
        const unsigned inputChannelMask = (1 << srcBits) - 1;

        // code from BetaFlight rx/crsf.cpp / bitpacker_unpack
        uint8_t bitsMerged = 0;
        uint32_t readValue = 0;

        Channels dest{};

        for (unsigned short & n : dest.data)
        {
            while (bitsMerged < srcBits)
            {
                uint8_t readByte = this->readByte();
                readValue |= ((uint32_t) readByte) << bitsMerged;
                bitsMerged += 8;
            }
            //printf("rv=%x(%x) bm=%u\n", readValue, (readValue & inputChannelMask), bitsMerged);
            n = (readValue & inputChannelMask);
            readValue >>= srcBits;
            bitsMerged -= srcBits;
        }
        return dest;
    }

    float Channels::operator[](int idx) {
        assert_param(idx >= 0 && idx < 16); // valid channel
        return (data[idx]-992)/819.0f; // map controller input to a -1 .. 1 range float
    }
} // crsf