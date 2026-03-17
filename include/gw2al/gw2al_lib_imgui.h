#pragma once

#include "gw2al_api.h"
#include <imgui.h>

/* lib_imgui dependency entry for gw2-addon-loader */
inline gw2al_addon_dsc LIB_IMGUI_DEP_ENTRY = {
    L"lib_imgui",
    L" ",
    2, 0, 1, 0,
    nullptr
};

/* Obtain ImGui context from lib_imgui via gw2al */
inline ImGuiContext* gw2al_get_imgui_context(gw2al_core_vtable* core) {
    auto hash = core->hash_name(L"lib_imgui_get_context");
    auto fn = (ImGuiContext*(*)())core->query_function(hash);
    if (fn) {
        return fn();
    }
    return nullptr;
}

/* Subscribe to lib_imgui draw event */
inline void gw2al_subscribe_imgui_draw(gw2al_core_vtable* core, void* callback) {
    auto hash = core->hash_name(L"lib_imgui_draw_event");
    core->subscribe_event(hash, callback);
}

inline void gw2al_unsubscribe_imgui_draw(gw2al_core_vtable* core, void* callback) {
    auto hash = core->hash_name(L"lib_imgui_draw_event");
    core->unsubscribe_event(hash, callback);
}
