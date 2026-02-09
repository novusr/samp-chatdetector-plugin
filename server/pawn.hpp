#pragma once

#include <vector>

#include <types.hpp>

#include "pawn/amx/amx.h"
#include "pawn/plugincommon.h"

struct AmxScript;

static std::vector<AmxScript> gScripts;

struct AmxScript {

    AmxScript() = delete;
    ~AmxScript() noexcept = default;
    AmxScript(const AmxScript&) noexcept = default;
    AmxScript(AmxScript&&) noexcept = default;
    AmxScript& operator=(const AmxScript&) noexcept = default;
    AmxScript& operator=(AmxScript&&) noexcept = default;

public:

    explicit AmxScript(AMX* const amx, const int callbackIndex) noexcept
        : _amx{amx}
        , _callbackIndex{callbackIndex}
    {}

public:

    static bool Register(AMX* const amx) noexcept
    {
        int callbackIndex = -1;
        amx_FindPublic(amx, "OnPlayerTypingStateChange", &callbackIndex);

        if (callbackIndex == -1)
            return false;

        gScripts.emplace_back(amx, callbackIndex);
        return true;
    }

public:

    void OnPlayerTypingStateChange(const uword_t player, const bool isTyping) noexcept
    {
        if (_callbackIndex == -1) return;

        amx_Push(_amx, static_cast<cell>(isTyping ? 1 : 0));
        amx_Push(_amx, static_cast<cell>(player));
        amx_Exec(_amx, nullptr, _callbackIndex);
    }

private:

    AMX* _amx;
    int _callbackIndex;

};

inline void OnPlayerTypingStateChange(const uword_t player, const bool isTyping) noexcept
{
    for (auto& script : gScripts)
        script.OnPlayerTypingStateChange(player, isTyping);
}
