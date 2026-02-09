#pragma once

#include <types.hpp>
#include <packets.hpp>
#include <jump_hook.hpp>
#include <scanner.hpp>

#include "raknet/bitstream.h"
#include "raknet/rakclient.h"

struct Network {

    Network(const Network&) = delete;
    Network(Network&&) = delete;
    Network& operator=(const Network&) = delete;
    Network& operator=(Network&&) = delete;

private:

    Network() noexcept = default;
    ~Network() noexcept = default;

public:

    static Network& Instance() noexcept
    {
        static Network instance;
        return instance;
    }

public:

    bool Initialize(const cptr_t base) noexcept
    {
        static const char kGetRakClientInterfacePattern[] =
            "\x50\x00\x00\x00\x00\x00\x00\x00\x51\x68\x00\x00\x00\x00\xE8\x00\x00\x00"
            "\x00\x83\xC4\x04\x89\x04\x24\x85\xC0\xC7\x44\x00\x00\x00\x00\x00\x00\x74\x1F";
        static const char kGetRakClientInterfaceMask[] =
            "x???????xx????x????xxxxxxxxxx??????xx";

        const auto [address, length] = GetModuleInfo(base);
        if (address == nullptr) return false;

        const auto pointer = Scanner(address, length).Find
            (kGetRakClientInterfacePattern, kGetRakClientInterfaceMask);
        if (pointer == nullptr) return false;

        return _hook.Initialize(static_cast<adr_t>(pointer) - 13, GetRakClientInterfaceHook);
    }

    void Deinitialize() noexcept
    {
        _hook.Deinitialize();
        _client = nullptr;
    }

public:

    void SendTypingState(bool isTyping) noexcept
    {
        if (_client == nullptr) return;

        BitStream bs;
        bs.Write(static_cast<ubyte_t>(CustomPackets::ID_TYPING_STATE_CHANGE));
        bs.Write(static_cast<ubyte_t>(isTyping ? 1 : 0));

        _client->Send(&bs, PacketPriority::MEDIUM_PRIORITY,
            PacketReliability::RELIABLE_ORDERED, 0);
    }

private:

    static void GetRakClientInterfaceHook() noexcept;

private:

    RakClientInterface* _client = nullptr;
    JumpHook _hook;

};

void __declspec(naked) Network::GetRakClientInterfaceHook() noexcept
{
    static ptr_t temp;

    __asm
    {
        pushad
        mov ebp, esp
        sub esp, __LOCAL_SIZE
    }

    temp = Instance()._hook.Address();
    Instance()._hook.Deinitialize();
    temp = Instance()._client = static_cast<RakClientInterface * (*)()>(temp)();

    __asm
    {
        mov esp, ebp
        popad
        mov eax, temp
        ret
    }
}
