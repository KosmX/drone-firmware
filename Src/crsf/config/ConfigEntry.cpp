
#include "ConfigEntry.h"

#include "crsf/crsf_protocol.h"

const std::string &cfg::ConfigEntry::getName() const {
    return name;
}

unsigned char cfg::Uint8Entry::getTypeID() const {
    return crsf_value_type_e::CRSF_UINT8;
}


uint8_t cfg::Uint8Entry::get() const {
    return v;
}

bool cfg::BoolConfig::get() const {
    return Uint8Entry::get() != 0;
}
