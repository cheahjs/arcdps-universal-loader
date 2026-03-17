#pragma once

#include <windows.h>
#include <string>
#include <arcdps/arcdps_structs.h>

class AddonInstance {
public:
    bool Load(const std::wstring& path, void* imguiCtx, HMODULE loaderModule);
    void Unload();

    void CallImgui(uint32_t notCharselOrLoading);
    void CallOptionsEnd();
    void CallOptionsWindows(const char* windowName);
    uint32_t CallWndProcNoFilter(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
    uint32_t CallWndProcFilter(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
    void CallCombat(cbtevent* ev, ag* src, ag* dst, const char* skillname, uint64_t id, uint64_t revision);
    void CallCombatLocal(cbtevent* ev, ag* src, ag* dst, const char* skillname, uint64_t id, uint64_t revision);

    const char* GetName() const { return m_name; }
    bool IsLoaded() const { return m_module != nullptr; }

private:
    HMODULE m_module = nullptr;
    arcdps_exports* m_exports = nullptr;
    get_release_addr_t m_releaseAddr = nullptr;
    const char* m_name = "unknown";
};
