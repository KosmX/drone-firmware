//
// Created by kosmx on 11/18/23.
//

#include "CRSF_listener.h"
#include <stdexcept>
#include "telemetry.h"
#include "crc.h"

namespace crsf {

    Packet::Packet(os::RingBufferEntryPtr pData): pData(pData) {}

    size_t Packet::getLength() const {
        return pData[1];
    }

    char Packet::getPacketId() const {
        return pData[2];
    }

    os::RingBufferEntryPtr Packet::getDataPtr() const {
        return isExtended() ? pData + 5 : pData + 3;
    }

    bool Packet::isExtended() const {
        return getPacketId() >= 0x28;
    }

    uint8_t Packet::getCrc() const {
        return pData[getLength() + 1];
    }

    /**
     * Small map, performance might be bad..
     */
    const std::map<crsf_frame_type_e, HandlerFunction> handlers{
            {
                crsf_frame_type_e::CRSF_FRAMETYPE_GPS, [](const Packet& config) -> bool {return false;},
            }
    };

    int checkForData(os::uart_dma &uart) {
        int packets = 0;

        while (uart.available() >= 2) {
            auto len = uart.peek(1);
            if (uart.available() < CRSF_FRAME_SIZE(len)) break; // not enough data yet

            Packet conf{uart.read(CRSF_FRAME_SIZE(len))};
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

}