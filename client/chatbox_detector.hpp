//Detects SA-MP chatbox open/close state based on key presses.

#pragma once

#include <cstdint>
#include <Windows.h>

struct ChatboxDetector {

    ChatboxDetector(const ChatboxDetector&) = delete;
    ChatboxDetector(ChatboxDetector&&) = delete;
    ChatboxDetector& operator=(const ChatboxDetector&) = delete;
    ChatboxDetector& operator=(ChatboxDetector&&) = delete;

private:

    ChatboxDetector() noexcept = default;
    ~ChatboxDetector() noexcept = default;

public:

    static ChatboxDetector& Instance() noexcept
    {
        static ChatboxDetector instance;
        return instance;
    }

public:

    // Callback type for chatbox state changes
    // isOpen: true if chatbox opened, false if closed
    using StateCallback = void(*)(bool isOpen);

    void Initialize(StateCallback callback) noexcept
    {
        _callback = callback;
        _chatboxOpen = false;
    }

    void Deinitialize() noexcept
    {
        _callback = nullptr;
        _chatboxOpen = false;
    }

    void OnKeyEvent(uint32_t vkCode, bool pressed) noexcept
    {
        if (!pressed) return;

        if (!_chatboxOpen)
        {
            if (vkCode == 'T' || vkCode == VK_OEM_3 || vkCode == VK_F6)
            {
                _chatboxOpen = true;
                if (_callback != nullptr)
                    _callback(true);
            }
        }
        else
        {
            if (vkCode == VK_ESCAPE || vkCode == VK_RETURN || vkCode == VK_F6)
            {
                _chatboxOpen = false;
                if (_callback != nullptr)
                    _callback(false);
            }
        }
    }

    bool IsChatboxOpen() const noexcept
    {
        return _chatboxOpen;
    }

private:

    StateCallback _callback = nullptr;
    bool _chatboxOpen = false;

};
