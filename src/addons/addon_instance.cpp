#include "addon_instance.h"
#include <cstdlib>

bool AddonInstance::Load(const std::wstring& path, void* imguiCtx, HMODULE loaderModule) {
    m_module = LoadLibraryW(path.c_str());
    if (!m_module) return false;

    auto getInit = (get_init_addr_t)GetProcAddress(m_module, "get_init_addr");
    if (!getInit) {
        FreeLibrary(m_module);
        m_module = nullptr;
        return false;
    }

    m_releaseAddr = (get_release_addr_t)GetProcAddress(m_module, "get_release_addr");

    // Call get_init_addr — pass our HMODULE so addons can GetProcAddress(arcdll, "e3") against us
    m_exports = getInit("0.0.0.0", imguiCtx, nullptr, loaderModule, (void*)malloc, (void*)free, 11);
    if (m_exports) {
        m_name = m_exports->out_name ? m_exports->out_name : "unknown";
    }

    return true;
}

void AddonInstance::Unload() {
    if (m_releaseAddr) {
        m_releaseAddr();
    }
    if (m_module) {
        FreeLibrary(m_module);
        m_module = nullptr;
    }
    m_exports = nullptr;
    m_releaseAddr = nullptr;
}

void AddonInstance::CallImgui(uint32_t notCharselOrLoading) {
    if (m_exports && m_exports->imgui) {
        ((imgui_callback_t)m_exports->imgui)(notCharselOrLoading);
    }
}

void AddonInstance::CallOptionsEnd() {
    if (m_exports && m_exports->options_end) {
        ((options_callback_t)m_exports->options_end)();
    }
}

void AddonInstance::CallOptionsWindows(const char* windowName) {
    if (m_exports && m_exports->options_windows) {
        ((options_windows_callback_t)m_exports->options_windows)(windowName);
    }
}

uint32_t AddonInstance::CallWndProcNoFilter(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    if (m_exports && m_exports->wnd_nofilter) {
        return ((wndproc_callback_t)m_exports->wnd_nofilter)(hwnd, msg, wparam, lparam);
    }
    return 0;
}

uint32_t AddonInstance::CallWndProcFilter(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    if (m_exports && m_exports->wnd_filter) {
        return ((wndproc_callback_t)m_exports->wnd_filter)(hwnd, msg, wparam, lparam);
    }
    return 0;
}

void AddonInstance::CallCombat(cbtevent* ev, ag* src, ag* dst, const char* skillname, uint64_t id, uint64_t revision) {
    if (m_exports && m_exports->combat) {
        ((combat_callback_t)m_exports->combat)(ev, src, dst, skillname, id, revision);
    }
}

void AddonInstance::CallCombatLocal(cbtevent* ev, ag* src, ag* dst, const char* skillname, uint64_t id, uint64_t revision) {
    if (m_exports && m_exports->combat_local) {
        ((combat_callback_t)m_exports->combat_local)(ev, src, dst, skillname, id, revision);
    }
}
