#pragma once

#include <windows.h>
#include <dxgi.h>
#include <d3d11.h>

struct ImGuiContext;

namespace ImGuiImpl {
    void Init(IDXGISwapChain* swapchain, ID3D11Device* device, ID3D11DeviceContext* context, HWND hwnd);
    void Shutdown();
    void NewFrame();
    void Render();
    void* GetContext();

    // For gw2al/Nexus: use an externally provided context
    void SetExternalContext(void* ctx);
}
