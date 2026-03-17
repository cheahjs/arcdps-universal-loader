#pragma once

#include <windows.h>
#include <dxgi.h>
#include <d3d11.h>

enum class LoadMethod {
    Proxy,
    Nexus,
    GW2AL,
    GW2Load,
};

namespace LoaderCore {
    void SetLoadMethod(LoadMethod method);
    LoadMethod GetLoadMethod();

    void OnPresent(IDXGISwapChain* swapchain);
    void Activate();
    void Shutdown();

    bool IsActivated();
    bool IsDisabled();
}
