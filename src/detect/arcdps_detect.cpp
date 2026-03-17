#include "arcdps_detect.h"
#include <windows.h>
#include <tlhelp32.h>
#include <string>

namespace ArcdpsDetect {

static HMODULE GetOurModule() {
    HMODULE hm = nullptr;
    GetModuleHandleExW(
        GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
        (LPCWSTR)&GetOurModule,
        &hm
    );
    return hm;
}

bool IsArcdpsPresent() {
    HMODULE ourModule = GetOurModule();

    // Enumerate all loaded modules and check for arcdps exports
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, 0);
    if (snap == INVALID_HANDLE_VALUE) {
        return false;
    }

    MODULEENTRY32W me{};
    me.dwSize = sizeof(me);

    if (Module32FirstW(snap, &me)) {
        do {
            // Skip our own module
            if (me.hModule == ourModule) continue;

            // Check for arcdps-specific exports
            auto e0 = GetProcAddress(me.hModule, "e0");
            auto e3 = GetProcAddress(me.hModule, "e3");
            if (e0 && e3) {
                CloseHandle(snap);
                return true;
            }
        } while (Module32NextW(snap, &me));
    }

    CloseHandle(snap);

    // Also check if bin64/d3d11.dll exists with arcdps signatures
    // (relevant when we're NOT the d3d11 proxy)
    wchar_t exePath[MAX_PATH];
    GetModuleFileNameW(nullptr, exePath, MAX_PATH);
    std::wstring dir(exePath);
    auto lastSlash = dir.find_last_of(L'\\');
    if (lastSlash != std::wstring::npos) {
        dir = dir.substr(0, lastSlash + 1);
    }
    std::wstring d3d11Path = dir + L"d3d11.dll";

    // Try to load d3d11.dll without executing code to check exports
    HMODULE hTest = LoadLibraryExW(d3d11Path.c_str(), nullptr, DONT_RESOLVE_DLL_REFERENCES);
    if (hTest && hTest != ourModule) {
        auto e0 = GetProcAddress(hTest, "e0");
        auto e3 = GetProcAddress(hTest, "e3");
        bool isArcdps = (e0 != nullptr && e3 != nullptr);
        FreeLibrary(hTest);
        if (isArcdps) return true;
    } else if (hTest) {
        FreeLibrary(hTest);
    }

    return false;
}

} // namespace ArcdpsDetect
