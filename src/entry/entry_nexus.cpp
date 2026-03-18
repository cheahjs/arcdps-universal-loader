#include "entry_nexus.h"
#include <nexus/Nexus.h>
#include "core/loader_core.h"
#include "imgui/imgui_impl.h"
#include "addons/addon_manager.h"

static AddonAPI* s_nexusApi = nullptr;

static void OnNexusLoad(AddonAPI* api) {
    s_nexusApi = api;

    LoaderCore::SetLoadMethod(LoadMethod::Nexus);

    // Use Nexus's ImGui context
    if (api->ImguiContext) {
        ImGuiImpl::SetExternalContext(api->ImguiContext);
    }

    // Register render callback
    api->RegisterRender(EntryNexus::OnRender);
}

static void OnNexusUnload() {
    if (s_nexusApi) {
        s_nexusApi->DeregisterRender(EntryNexus::OnRender);
    }
    LoaderCore::Shutdown();
    s_nexusApi = nullptr;
}

namespace EntryNexus {

void OnRender() {
    // Nexus gives us a render callback each frame — use it as our Present equivalent
    // We pass nullptr for swapchain since Nexus manages rendering
    LoaderCore::OnPresent(nullptr);
}

} // namespace EntryNexus

// Nexus addon definition
static AddonDefinition s_addonDef = {
    .Signature = -996748,  // unique negative signature for unofficial addons
    .APIVersion = -1,
    .Name = "arcdps Universal Loader",
    .Version = {0, 1, 0, 0},
    .Author = "arcdps-universal-loader",
    .Description = "Loads arcdps addon DLLs when arcdps is unavailable",
    .Load = OnNexusLoad,
    .Unload = OnNexusUnload,
    .Flags = EAddonFlags::None,
    .Provider = EUpdateProvider::None,
    .UpdateLink = nullptr,
};

extern "C" AddonDefinition* GetAddonDef() {
    return &s_addonDef;
}
