#pragma once

#include "types.hpp"

// ----------------------------------------------------------------

using Version = udword_t;

// ----------------------------------------------------------------

using VersionMajor = ubyte_t;
using VersionMinor = ubyte_t;
using VersionPatch = uword_t;

// ----------------------------------------------------------------

constexpr VersionMajor GetVersionMajor(const Version version) noexcept
{
    return static_cast<VersionMajor>(version >> 24);
}

constexpr VersionMinor GetVersionMinor(const Version version) noexcept
{
    return static_cast<VersionMinor>(version >> 16 & 0xFF);
}

constexpr VersionPatch GetVersionPatch(const Version version) noexcept
{
    return static_cast<VersionPatch>(version & 0xFFFF);
}

// ----------------------------------------------------------------

constexpr Version MakeVersion(const VersionMajor major,
                              const VersionMinor minor,
                              const VersionPatch patch) noexcept
{
    return static_cast<Version>(major) << 24 |
           static_cast<Version>(minor) << 16 |
           static_cast<Version>(patch);
}
