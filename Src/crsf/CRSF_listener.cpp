//
// Created by kosmx on 11/18/23.
//

#include "CRSF_listener.h"
#include <stdexcept>
#include "telemetry.h"
#include "crc.h"

namespace crsf {

    RxPacket::RxPacket(os::RingBufferEntryPtr pData): pData(pData) {}

    size_t RxPacket::getLength() const {
        return pData[1];
    }

    char RxPacket::getPacketId() const {
        return pData[2];
    }

    os::RingBufferEntryPtr RxPacket::getDataPtr() const {
        return isExtended() ? pData + 5 : pData + 3;
    }

    bool RxPacket::isExtended() const {
        return getPacketId() >= 0x28;
    }

    uint8_t RxPacket::getCrc() const {
        return pData[getLength() + 1];
    }

    /**
     * Small map, performance might be bad..
     */
    const std::map<crsf_frame_type_e, HandlerFunction> handlers{
            {
                crsf_frame_type_e::CRSF_FRAMETYPE_GPS, [](const RxPacket& config) -> bool {return false;},
            }
    };

    int checkForData(os::uart_dma &uart) {
        int packets = 0;

        while (uart.available() >= 2) {
            auto len = uart.peek(1);
            if (uart.available() < CRSF_FRAME_SIZE(len)) break; // not enough data yet

            RxPacket conf{uart.read(CRSF_FRAME_SIZE(len))};
            // TODO verify CRC

            auto crc = GenericCRC8::PolyD5.calc((conf.pData + 2), conf.getLength() + 1);
            if (crc != conf.getCrc()) {
                tlm::ITelemetry::INSTANCE->log(std::make_unique<std::string>("CRC doesn't match for packet"));
            }

            try {
                handlers.at(static_cast<crsf_frame_type_e>(conf.getPacketId()))(conf);
            } catch (std::out_of_range& e) {
                tlm::ITelemetry::INSTANCE->log(std::make_unique<std::string>("Unknown packet ID: " + std::to_string(conf.getPacketId())));
            }
        }
        return packets;
    }

    TxPacket::TxPacket(uint8_t *data): pData(data) {
        pData[0] = 0x8c;

    }

    void TxPacket::calcCRCAndSetPacketLength(size_t length) {
        size_t l = length;
        if (isExtended()) l += 4;
        else l += 2;
        pData[1] = l;
        auto crc = GenericCRC8::PolyD5.calc((pData + 2), length + 1);
        pData[l - 1] = crc;
    }

    void TxPacket::setPacketType(uint8_t id) {
        pData[2] = id;
    }

    void TxPacket::setExtDest(uint8_t d) {
        pData[3] = d;
    }

    void TxPacket::setExtSrc(uint8_t s) {
        pData[4] = s;
    }

    bool TxPacket::isExtended() const {
        return getPacketType() >= 0x28;
    }

    uint8_t TxPacket::getPacketType() const {
        return pData[2];
    }

    size_t TxPacket::getPacketOffset() const {
        return isExtended() ? 5 : 3;
    }

    uint8_t *TxPacket::getDataPtr() {
        return pData;
    }

    uint8_t *TxPacket::data() {
        return pData + getPacketOffset();
    }
}