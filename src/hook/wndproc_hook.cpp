#include "wndproc_hook.h"
#include "addons/addon_manager.h"
#include <imgui_impl_win32.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace WndProcHook {

static HWND s_hwnd = nullptr;
static WNDPROC s_originalWndProc = nullptr;

static LRESULT CALLBACK HookedWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    // Let ImGui handle input first
    if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) {
        return TRUE;
    }

    // Dispatch to addons (wnd_nofilter and wnd_filter)
    if (AddonManager::DispatchWndProc(hwnd, msg, wparam, lparam)) {
        return 0;
    }

    return CallWindowProcW(s_originalWndProc, hwnd, msg, wparam, lparam);
}

void Install(HWND hwnd) {
    if (s_hwnd) return;
    s_hwnd = hwnd;
    s_originalWndProc = (WNDPROC)SetWindowLongPtrW(hwnd, GWLP_WNDPROC, (LONG_PTR)HookedWndProc);
}

void Remove() {
    if (!s_hwnd) return;
    SetWindowLongPtrW(s_hwnd, GWLP_WNDPROC, (LONG_PTR)s_originalWndProc);
    s_originalWndProc = nullptr;
    s_hwnd = nullptr;
}

HWND GetHwnd() {
    return s_hwnd;
}

} // namespace WndProcHook
