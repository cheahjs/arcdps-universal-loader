#pragma once

#include <windows.h>
#include <string>

namespace AddonManager {
    void LoadAll(const std::wstring& baseDir);
    void UnloadAll();

    void DispatchPresent();
    bool DispatchWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
    void DispatchOptionsEnd();
}
