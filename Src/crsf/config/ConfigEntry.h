//
// Created by kosmx on 3/22/24.
//

#ifndef DRONE_FW_CONFIGENTRY_H
#define DRONE_FW_CONFIGENTRY_H
#include <memory>
#include <utility>
#include "ConfigResponseBuilder.h"
#include "ConfigUpdatePacket.h"
#include <limits>

namespace cfg {

    class ConfigEntry {
    public:
        ConfigEntry(const ConfigEntry &) = delete;
        void operator=(const ConfigEntry &) = delete;
    private:
        const std::string name;
        bool hidden = false;

    protected:
        explicit ConfigEntry(std::string name): name(std::move(name)) {}


    public:
        [[nodiscard]] virtual unsigned char getTypeID() const = 0;
        [[nodiscard]] const std::string& getName() const;

        [[nodiscard]] bool isHidden() const;
        void setHidden(bool);

        /**
         * Create a reply message
         * @param buf Message buffer
         */
        virtual void updateConfigFrom(ConfigUpdatePacket& buf) = 0;

        /**
         * Containing new config data.
         * This should update the config entry
         * @param buf
         */
        virtual void read(ConfigResponseBuilder& buf) const = 0;


    };


    class Uint8Entry : public ConfigEntry {
    private:
        uint8_t v;
        uint8_t min;
        uint8_t max;
    public:
        Uint8Entry(const std::string& name, uint8_t defVal, uint8_t min = std::numeric_limits<uint8_t>::min(), uint8_t max = std::numeric_limits<uint8_t>::max()):
        ConfigEntry(name), v(defVal), min(min), max(max) {}
        [[nodiscard]] unsigned char getTypeID() const override;

        [[nodiscard]] uint8_t get() const;

        void read(cfg::ConfigResponseBuilder&) const override;
        void updateConfigFrom(ConfigUpdatePacket &) override;
    };

    class BoolConfig : public Uint8Entry {
    public:
        BoolConfig(const std::string& name, bool defVal) : Uint8Entry(name, defVal, 0, 1) {}

        // hide super function? YES, I want that...
        // maybe OOP was a bad choice
        [[nodiscard]] bool get() const;
    };

}
#endif //DRONE_FW_CONFIGENTRY_H
