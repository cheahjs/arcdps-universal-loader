#pragma once

#include <windows.h>
#include <string>

namespace AddonManager {
    void LoadAll(const std::wstring& baseDir);
    void UnloadAll();

    void DispatchPresent();
    bool DispatchWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
    void DispatchOptionsEnd();
    void DispatchOptionsWindows(const char* windowName);

    // Renders a tab per loaded addon in the Extensions tab; calls options_end when selected
    void DispatchExtensionTabs();

    // Lists loaded addon names in the About tab
    void DispatchAboutInfo();
}
