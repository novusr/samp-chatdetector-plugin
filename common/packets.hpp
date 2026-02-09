#ifndef PACKETS_HPP
#define PACKETS_HPP

#include <stdint.h>

constexpr uint8_t ID_CUSTOM_BASE = 250;

enum CustomPackets : uint8_t
{
    ID_TYPING_STATE_CHANGE = ID_CUSTOM_BASE 
};

#endif
