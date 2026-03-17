#include <windows.h>
#include <string>
#include <algorithm>

#include "core/singleton.h"
#include "core/loader_core.h"
#include "proxy/d3d11_proxy.h"
#include "proxy/dxgi_proxy.h"
#include "proxy/version_proxy.h"

static bool g_active = false;

enum class ProxyMode {
    None,
    D3D11,
    DXGI,
    Version,
};

static ProxyMode DetectProxyMode() {
    wchar_t path[MAX_PATH];
    HMODULE hm = nullptr;
    GetModuleHandleExW(
        GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
        (LPCWSTR)&DetectProxyMode,
        &hm
    );
    GetModuleFileNameW(hm, path, MAX_PATH);

    std::wstring filename(path);
    auto lastSlash = filename.find_last_of(L'\\');
    if (lastSlash != std::wstring::npos) {
        filename = filename.substr(lastSlash + 1);
    }

    // Convert to lowercase for comparison
    std::transform(filename.begin(), filename.end(), filename.begin(), ::towlower);

    if (filename == L"d3d11.dll") return ProxyMode::D3D11;
    if (filename == L"dxgi.dll") return ProxyMode::DXGI;
    if (filename == L"version.dll") return ProxyMode::Version;
    return ProxyMode::None;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID lpReserved) {
    (void)hModule;
    (void)lpReserved;

    switch (reason) {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hModule);

        // Singleton guard
        if (!Singleton::TryAcquire()) {
            g_active = false;
            return TRUE;
        }
        g_active = true;

        // Detect proxy mode and initialize proxy forwarding
        {
            ProxyMode mode = DetectProxyMode();
            switch (mode) {
            case ProxyMode::D3D11:
                D3D11Proxy::Init();
                LoaderCore::SetLoadMethod(LoadMethod::Proxy);
                break;
            case ProxyMode::DXGI:
                DXGIProxy::Init();
                LoaderCore::SetLoadMethod(LoadMethod::Proxy);
                break;
            case ProxyMode::Version:
                VersionProxy::Init();
                LoaderCore::SetLoadMethod(LoadMethod::Proxy);
                break;
            case ProxyMode::None:
                // Loaded as managed addon (Nexus/gw2al/GW2Load) — entry points handle setup
                break;
            }
        }
        break;

    case DLL_PROCESS_DETACH:
        if (g_active) {
            LoaderCore::Shutdown();
            D3D11Proxy::Shutdown();
            DXGIProxy::Shutdown();
            VersionProxy::Shutdown();
            Singleton::Release();
        }
        break;
    }

    return TRUE;
}
