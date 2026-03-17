#include "entry_proxy.h"
#include "core/loader_core.h"
#include "hook/d3d11_hook.h"

namespace EntryProxy {

void OnSwapChainCreated(IDXGISwapChain* swapchain) {
    LoaderCore::SetLoadMethod(LoadMethod::Proxy);
    D3D11Hook::Install(swapchain);
}

} // namespace EntryProxy
