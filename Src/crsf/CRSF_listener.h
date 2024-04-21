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

    class TxPacket {
        uint8_t* pData;

    public:
        TxPacket(const TxPacket&) = delete;
        TxPacket& operator=(const TxPacket&) = delete;

        explicit TxPacket(uint8_t* data);

        void calcCRCAndSetPacketLength(size_t length);
        [[nodiscard]] uint8_t getPacketType() const;
        void setPacketType(uint8_t id);

        void setExtDest(uint8_t);
        void setExtSrc(uint8_t);
        [[nodiscard]] bool isExtended() const;

        [[nodiscard]] size_t getPacketOffset() const;
        uint8_t* getDataPtr();
        uint8_t* data();

    };

    class RxPacket {
        // describe what to do with specific packet
    public:
        [[nodiscard]] char getPacketId() const;
        [[nodiscard]] size_t getLength() const;

        /**
         * Get data address
         * @return calculated data, may be overwritten
         */
        [[nodiscard]] os::RingBufferEntryPtr data() const;

        [[nodiscard]] os::RingBufferEntryPtr getDataPtr() const;

        [[nodiscard]] bool isExtended() const;

        const os::RingBufferEntryPtr pData;

        explicit RxPacket(os::RingBufferEntryPtr pData);

        [[nodiscard]] uint8_t getCrc() const;
    };

    // RxPacket handling will be a procedural switch case.
    // OOP sucks for many reasons
    typedef std::function<bool(const RxPacket& config)> HandlerFunction;

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
