//
// Created by kosmx on 11/18/23.
//

#ifndef STUFF_CRSF_LISTENER_H
#define STUFF_CRSF_LISTENER_H
#ifdef __cplusplus
#include <functional>
#include "os/uart_dma.h"
#include "crsf/crsf_protocol.h" 
#include <map>

namespace crsf {


    class Packet {
        // describe what to do with specific packet
    public:
        [[nodiscard]] char getPacketId() const;
        [[nodiscard]] size_t getLength() const;
        [[nodiscard]] os::RingBufferEntryPtr getDataPtr() const;

        [[nodiscard]] bool isExtended() const;

        const os::RingBufferEntryPtr pData;

        explicit Packet(os::RingBufferEntryPtr pData);

        [[nodiscard]] uint8_t getCrc() const;
    };

    // Packet handling will be a procedural switch case.
    // OOP sucks for many reasons
    typedef std::function<bool(const Packet& config)> HandlerFunction;

    extern const std::map<crsf_frame_type_e, HandlerFunction> handlers;

    /**
     * BLOCKING
     * init ELRS connection
     * return when connection is active
     */
    void initComm();

    /**
     * Handle received data, including validation and processing
     * @param pData volatile pointer to data. Its content is safe for processing, but is going to be invalid after handlePacket returns
     */
    void handlePacket(os::RingBufferEntryPtr pData);

    /**
     * Check for new data, and if there is any, handle it
     * @param uart handler
     * @return amount of packets handled
     */
    int checkForData(os::uart_dma& uart);
}

#endif //__cplusplus
#endif //STUFF_CRSF_LISTENER_H
