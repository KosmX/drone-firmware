
#include "ConfigEntry.h"

#include "crsf/crsf_protocol.h"

namespace cfg {

    const std::string &ConfigEntry::getName() const {
        return name;
    }

    bool ConfigEntry::isHidden() const {
        return hidden;
    }

    void ConfigEntry::setHidden(bool h) {
        hidden = h;
    }


    unsigned char Uint8Entry::getTypeID() const {
        return crsf_value_type_e::CRSF_UINT8;
    }


    uint8_t Uint8Entry::get() const {
        return v;
    }

    void Uint8Entry::read(ConfigResponseBuilder &reply) const {
        reply.writeByte(v);
        reply.writeByte(min);
        reply.writeByte(max);
    }

    void Uint8Entry::updateConfigFrom(ConfigUpdatePacket& packet) {
        this->v = packet.data()[0];
    }

    bool BoolConfig::get() const {
        return Uint8Entry::get() != 0;
    }
}