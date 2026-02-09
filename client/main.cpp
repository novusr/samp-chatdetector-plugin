#include <Windows.h>
#include <atomic>

#include "network.hpp"
#include "keyboard_hook.hpp"
#include "chatbox_detector.hpp"

static HANDLE g_MessagePumpThread = nullptr;
static std::atomic<int> g_TypingState{0}; // 0 = no change, 1 = started, 2 = stopped

static void OnChatboxStateChange(bool isOpen) noexcept {
    g_TypingState.store(isOpen ? 1 : 2);
}

static bool IsGameWindowFocused() noexcept {
    HWND foreground = GetForegroundWindow();
    if (foreground == nullptr) return false;
    
    DWORD pid = 0;
    GetWindowThreadProcessId(foreground, &pid);
    return pid == GetCurrentProcessId();
}

static void OnKeyEvent(uint32_t vkCode, bool pressed) noexcept {
    if (!IsGameWindowFocused()) return;
    ChatboxDetector::Instance().OnKeyEvent(vkCode, pressed);
}

static DWORD WINAPI MessagePumpThread(LPVOID) noexcept {
    MSG msg;
    SetTimer(NULL, 1, 50, nullptr);

    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        if (msg.message == WM_TIMER) {
            int state = g_TypingState.exchange(0);
            if (state == 1)
                Network::Instance().SendTypingState(true);
            else if (state == 2)
                Network::Instance().SendTypingState(false);
        }
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    return 0;
}

BOOL APIENTRY DllMain(const HMODULE hModule, const DWORD reason, const LPVOID) noexcept {
    if (reason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hModule);

        HMODULE sampModule = LoadLibraryW(L"samp.dll");
        if (sampModule == nullptr)
            return FALSE;

        if (!Network::Instance().Initialize(sampModule))
            return FALSE;

        ChatboxDetector::Instance().Initialize(OnChatboxStateChange);

        g_MessagePumpThread = CreateThread(nullptr, 0, MessagePumpThread, nullptr, 0, nullptr);
        if (g_MessagePumpThread == nullptr) {
            Network::Instance().Deinitialize();
            return FALSE;
        }

        Sleep(100);

        if (!KeyboardHook::Instance().Initialize(OnKeyEvent)) {
            Network::Instance().Deinitialize();
            return FALSE;
        }
    }
    else if (reason == DLL_PROCESS_DETACH) {
        KeyboardHook::Instance().Deinitialize();
        ChatboxDetector::Instance().Deinitialize();

        if (g_MessagePumpThread != nullptr) {
            PostThreadMessageW(GetThreadId(g_MessagePumpThread), WM_QUIT, 0, 0);
            WaitForSingleObject(g_MessagePumpThread, 1000);
            CloseHandle(g_MessagePumpThread);
        }

        Network::Instance().Deinitialize();
    }

    return TRUE;
}
