//
// Created by kosmx on 4/22/24.
//

#ifndef DRONE_FW_PACKETS_H
#define DRONE_FW_PACKETS_H

#include "CRSF_listener.h"

namespace crsf {

    class BatterySensor {

    };

    class HeartBeat: public TxPacket{
    public:
        static void write(TxPacket& p);
    };

    struct Channels {
        uint16_t data[16];
    };

    class ChannelData: public RxPacket {
    public:
        ChannelData(const RxPacket&);

        /**
         * @return channel data
         */
        Channels readChannels() const;
    };

} // crsf

#endif //DRONE_FW_PACKETS_H
