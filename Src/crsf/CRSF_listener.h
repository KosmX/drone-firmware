//
// Created by kosmx on 11/18/23.
//

#ifndef STUFF_CRSF_LISTENER_H
#define STUFF_CRSF_LISTENER_H
#ifdef __cplusplus

namespace crsf {

    class PacketConfig {
        // describe what to do with specific packet
    public:
        const char packetId;
        const bool isExtended;
    };
}

#endif //__cplusplus
#endif //STUFF_CRSF_LISTENER_H
