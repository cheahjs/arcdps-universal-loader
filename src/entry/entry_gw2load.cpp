#include "entry_gw2load.h"
#include <gw2load/gw2load_api.h>
#include "core/loader_core.h"
#include <dxgi.h>

namespace EntryGW2Load {

void OnPresent(void* swapchain) {
    LoaderCore::OnPresent((IDXGISwapChain*)swapchain);
}

} // namespace EntryGW2Load

extern "C" __declspec(dllexport) uint32_t GW2Load_GetAddonAPIVersion() {
    return GW2Load_CurrentAddonAPIVersion;
}

extern "C" __declspec(dllexport) void GW2Load_OnLoad(void* api) {
    (void)api;
    LoaderCore::SetLoadMethod(LoadMethod::GW2Load);
    // GW2Load provides Present callbacks — we'll receive them via the callback system
}

extern "C" __declspec(dllexport) void GW2Load_OnClose() {
    LoaderCore::Shutdown();
}
