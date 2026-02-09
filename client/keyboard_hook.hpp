#pragma once

#include <Windows.h>
#include <cstdint>

struct KeyboardHook {

    KeyboardHook(const KeyboardHook&) = delete;
    KeyboardHook(KeyboardHook&&) = delete;
    KeyboardHook& operator=(const KeyboardHook&) = delete;
    KeyboardHook& operator=(KeyboardHook&&) = delete;

private:

    KeyboardHook() noexcept = default;
    ~KeyboardHook() noexcept = default;

public:

    static KeyboardHook& Instance() noexcept
    {
        static KeyboardHook instance;
        return instance;
    }

public:
    using KeyCallback = void(*)(uint32_t vkCode, bool pressed);

    bool Initialize(KeyCallback callback) noexcept
    {
        if (_hook != nullptr)
            return false;

        _callback = callback;

        _hook = SetWindowsHookExW(
            WH_KEYBOARD_LL,
            LowLevelKeyboardProc,
            GetModuleHandleW(nullptr),
            0
        );

        return _hook != nullptr;
    }

    void Deinitialize() noexcept
    {
        if (_hook != nullptr)
        {
            UnhookWindowsHookEx(_hook);
            _hook = nullptr;
        }

        _callback = nullptr;
    }

    bool IsInitialized() const noexcept
    {
        return _hook != nullptr;
    }

private:

    static LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) noexcept
    {
        if (nCode == HC_ACTION && Instance()._callback != nullptr)
        {
            const auto* kbd = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);
            
            bool pressed = (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN);
            bool released = (wParam == WM_KEYUP || wParam == WM_SYSKEYUP);

            if (pressed || released)
            {
                Instance()._callback(kbd->vkCode, pressed);
            }
        }

        return CallNextHookEx(Instance()._hook, nCode, wParam, lParam);
    }

private:

    HHOOK _hook = nullptr;
    KeyCallback _callback = nullptr;

};
