//
// Created by kosmx on 11/18/23.
//

#ifndef STUFF_CRSF_LISTENER_H
#define STUFF_CRSF_LISTENER_H
#ifdef __cplusplus
#include <functional>
#include <string>
#include "os/uart_dma.h"
#include "crsf/crsf_protocol.h"

namespace crsf {

    class TxPacket {
        uint8_t* pData;
        size_t writeIdx = 0;

    public:
        TxPacket(const TxPacket&) = delete;
        TxPacket& operator=(const TxPacket&) = delete;

        explicit TxPacket(uint8_t* data);

        /**
         *
         * @param length data length
         */
        void calcCRCAndSetPacketLength(size_t length);

        /**
         * If packet was constructed with writeBytes, use the auto data length
         */
        void calcCRCAndSetPacketLength();
        [[nodiscard]] uint8_t getPacketType() const;
        void setPacketType(uint8_t id);

        void setExtDest(uint8_t);
        void setExtSrc(uint8_t);
        [[nodiscard]] bool isExtended() const;

        [[nodiscard]] size_t getPacketOffset() const;
        uint8_t* getDataPtr();
        uint8_t* data();

        void defaultRoute();

        void writeByte(uint8_t byte);

        void writeString(const std::string& str);

        void writeBytes(const uint8_t *data, size_t len, bool prefixSize = false);

        [[nodiscard]] size_t getWriteIndex() const;
    };

    class RxPacket {
        // describe what to do with specific packet
    private:
        mutable size_t readIdx = 0;
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

        uint8_t readByte() const;
    };
}

#endif //__cplusplus
#endif //STUFF_CRSF_LISTENER_H
