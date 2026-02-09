#include <version.hpp>
#include <packets.hpp>

#include "pawn.hpp"
#include "network.hpp"

constexpr Version kCurrentVersion = MakeVersion(1, 0, 0);

extern ptr_t pAMXFunctions;

static bool OnPacket(const uword_t player, const cptr_t data, const uint_t size) noexcept
{
    if (size < 2) return false;

    const ubyte_t packetId = static_cast<cadr_t>(data)[0];

    if (packetId != ID_TYPING_STATE_CHANGE)
        return false;

    const bool isTyping = static_cast<cadr_t>(data)[1] != 0;
    OnPlayerTypingStateChange(player, isTyping);

    return true;
}

PLUGIN_EXPORT bool PLUGIN_CALL Load(void* const* const ppData) noexcept
{
    pAMXFunctions = ppData[PLUGIN_DATA_AMX_EXPORTS];

    const auto logprintf = reinterpret_cast<void(*)(cstr_t,...)>(ppData[PLUGIN_DATA_LOGPRINTF]);

    Network::Instance().OnPacket = OnPacket;

    if (!Network::Instance().Initialize(ppData[PLUGIN_DATA_LOGPRINTF]))
    {
        logprintf("[SAMP-ChatDetector] : failed to initialize network module");
        return false;
    }

    logprintf("SAMP-ChatDetector v%hhu.%hhu.%hu loaded",
        GetVersionMajor(kCurrentVersion),
        GetVersionMinor(kCurrentVersion),
        GetVersionPatch(kCurrentVersion));

    return true;
}

PLUGIN_EXPORT void PLUGIN_CALL Unload() noexcept
{
    Network::Instance().Deinitialize();
}

PLUGIN_EXPORT int PLUGIN_CALL AmxLoad(AMX* const amx) noexcept
{
    AmxScript::Register(amx);
    return AMX_ERR_NONE;
}

PLUGIN_EXPORT int PLUGIN_CALL AmxUnload(AMX* const) noexcept
{
    return AMX_ERR_NONE;
}

PLUGIN_EXPORT unsigned int PLUGIN_CALL Supports() noexcept
{
    return SUPPORTS_VERSION | SUPPORTS_AMX_NATIVES;
}
