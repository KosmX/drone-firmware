#pragma once
#include <cstdint>
#include <memory>

const size_t crclen = 256;

class GenericCRC8
{
private:
    uint8_t crc8tab[crclen]{};
    uint8_t crcpoly{};

public:

    static const GenericCRC8 PolyD5;

    constexpr GenericCRC8(uint8_t poly);
    uint8_t calc(const uint8_t data) const;
    //uint8_t calc(const uint8_t *data, uint16_t len, uint8_t crc = 0);

    /**
     * Calc CRC for any data ptr type
     * @tparam T any array pointer (c++ iterator) type
     * @param data array iterator
     * @param len array length
     * @return crc
     */
    template<class T>
    uint8_t calc(T data, uint16_t len, uint8_t crc = 0) const
    {
        while (len--)
        {
            crc = crc8tab[crc ^ *data++];
        }
        return crc;
    }
};

class Crc2Byte
{
private:
    uint16_t _crctab[crclen];
    uint8_t  _bits = 0;
    uint16_t _bitmask = 0;
    uint16_t _poly = 0;

public:
    constexpr Crc2Byte(uint8_t bits, uint16_t poly);
    uint16_t calc(uint8_t *data, uint8_t len, uint16_t crc);
};
