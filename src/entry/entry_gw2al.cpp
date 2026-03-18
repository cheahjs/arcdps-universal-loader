#include "entry_gw2al.h"
#include <gw2al/gw2al_api.h>
#include <gw2al/gw2al_lib_imgui.h>
#include "core/loader_core.h"
#include "imgui/imgui_impl.h"

static gw2al_core_vtable* s_core = nullptr;

namespace EntryGW2AL {

void OnImGuiDraw(int* result) {
    (void)result;
    // Called each frame by lib_imgui draw event
    LoaderCore::OnPresent(nullptr);
}

} // namespace EntryGW2AL

// Dependencies: lib_imgui
static gw2al_addon_dsc s_libImguiDep = LIB_IMGUI_DEP_ENTRY;
static gw2al_addon_dsc* s_deps[] = { &s_libImguiDep, nullptr };

static gw2al_addon_dsc s_addonDesc = {
    L"arcdps_universal_loader",
    L"Loads arcdps addon DLLs when arcdps is unavailable",
    0, 1, 0, 0,
    &s_libImguiDep
};

extern "C" gw2al_addon_dsc* gw2addon_get_description() {
    return &s_addonDesc;
}

extern "C" gw2al_api_ret gw2addon_load(gw2al_core_vtable* core) {
    s_core = core;
    LoaderCore::SetLoadMethod(LoadMethod::GW2AL);

    // Get ImGui context from lib_imgui
    ImGuiContext* ctx = gw2al_get_imgui_context(core);
    if (ctx) {
        ImGuiImpl::SetExternalContext(ctx);
    }

    // Subscribe to lib_imgui draw event
    gw2al_subscribe_imgui_draw(core, (void*)EntryGW2AL::OnImGuiDraw);

    return GW2AL_OK;
}

extern "C" gw2al_api_ret gw2addon_unload(gw2al_core_vtable* core) {
    gw2al_unsubscribe_imgui_draw(core, (void*)EntryGW2AL::OnImGuiDraw);
    LoaderCore::Shutdown();
    s_core = nullptr;
    return GW2AL_OK;
}
