#include "crsf/crc.h"

constexpr GenericCRC8::GenericCRC8(uint8_t poly)
{
    uint8_t crc;

    for (uint16_t i = 0; i < crclen; i++)
    {
        crc = i;
        for (uint8_t j = 0; j < 8; j++)
        {
            crc = (crc << 1) ^ ((crc & 0x80) ? poly : 0);
        }
        crc8tab[i] = crc & 0xFF;
    }
}

uint8_t GenericCRC8::calc(const uint8_t data) const
{
    return crc8tab[data];
}

// Calculate LUT on PC, upload it to MCU
constexpr const GenericCRC8 GenericCRC8::PolyD5{0xd5};

/*
uint8_t GenericCRC8::calc(const uint8_t *data, uint16_t len, uint8_t crc)
{
    while (len--)
    {
        crc = crc8tab[crc ^ *data++];
    }
    return crc;
}*/

constexpr Crc2Byte::Crc2Byte(uint8_t bits, uint16_t poly): _crctab{} {
    if (bits == _bits && poly == _poly)
        return;
    _poly = poly;
    _bits = bits;
    _bitmask = (1 << _bits) - 1;
    uint16_t highbit = 1 << (_bits - 1);
    uint16_t crc;
    for (uint16_t i = 0; i < crclen; i++)
    {
        crc = i << (bits - 8);
        for (uint8_t j = 0; j < 8; j++)
        {
            crc = (crc << 1) ^ ((crc & highbit) ? poly : 0);
        }
        _crctab[i] = crc;
    }
}

uint16_t Crc2Byte::calc(uint8_t *data, uint8_t len, uint16_t crc)
{
    while (len--)
    {
        crc = (crc << 8) ^ _crctab[((crc >> (_bits - 8)) ^ (uint16_t) *data++) & 0x00FF];
    }
    return crc & _bitmask;
}
