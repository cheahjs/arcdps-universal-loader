#pragma once

#include <dxgi.h>
#include <d3d11.h>

namespace D3D11Hook {
    void Install(IDXGISwapChain* swapchain);
    void Remove();
}
