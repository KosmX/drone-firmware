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

    os::RingBufferEntryPtr RxPacket::data() const {
        return isExtended() ? pData + 5 : pData + 3;
    }

    bool RxPacket::isExtended() const {
        return getPacketId() >= 0x28;
    }

    uint8_t RxPacket::getCrc() const {
        return pData[getLength() + 1];
    }

    os::RingBufferEntryPtr RxPacket::getDataPtr() const {
        return pData;
    }

    uint8_t RxPacket::readByte() const {
        return data()[readIdx++];
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

    void TxPacket::defaultRoute() {
        setExtDest(CRSF_ADDRESS_RADIO_TRANSMITTER);
        setExtSrc(CRSF_ADDRESS_CRSF_TRANSMITTER);
    }

    void TxPacket::writeString(const std::string &str) {
        for (auto& c:str) {
            writeByte(c);
        }
        writeByte(0);
    }

    void TxPacket::writeByte(uint8_t byte) {
        data()[writeIdx++] = byte;
    }

    void TxPacket::writeBytes(const uint8_t *data, size_t len, bool prefixSize) {
        if (prefixSize) {
            writeByte(len);
        }
        for (int i = 0; i < len; i++) {
            writeByte(data[i]);
        }
    }

    void TxPacket::calcCRCAndSetPacketLength() {
        calcCRCAndSetPacketLength(writeIdx);
    }

    size_t TxPacket::getWriteIndex() const {
        return writeIdx;
    }

}