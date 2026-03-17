#pragma once

#include <windows.h>

namespace WndProcHook {
    void Install(HWND hwnd);
    void Remove();
    HWND GetHwnd();
}
