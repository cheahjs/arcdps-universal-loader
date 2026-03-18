#include "settings_window.h"
#include "addons/addon_manager.h"
#include <imgui.h>
#include <windows.h>

namespace SettingsWindow {

static bool s_visible = false;

void Render() {
    if (!s_visible) return;

    ImGui::SetNextWindowSizeConstraints(ImVec2(370, 200), ImVec2(600, 800));
    if (!ImGui::Begin("arcdps universal loader", &s_visible, ImGuiWindowFlags_NoCollapse)) {
        ImGui::End();
        return;
    }

    if (ImGui::BeginTabBar("SettingsTabBar")) {
        if (ImGui::BeginTabItem("Interface")) {
            ImGui::TextDisabled("Windows");
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
            // Dispatch options_windows for each built-in arcdps window name
            AddonManager::DispatchOptionsWindows("squad");
            AddonManager::DispatchOptionsWindows("dps");
            AddonManager::DispatchOptionsWindows("skills");
            AddonManager::DispatchOptionsWindows("metrics");
            AddonManager::DispatchOptionsWindows("error");
            // nullptr signals addons to add their own custom window toggles
            AddonManager::DispatchOptionsWindows(nullptr);
            ImGui::PopStyleVar();
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Extensions")) {
            if (ImGui::BeginTabBar("ExtensionsTabBar")) {
                AddonManager::DispatchExtensionTabs();
                ImGui::EndTabBar();
            }
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("About")) {
            ImGui::TextWrapped("arcdps universal loader v0.1.0");
            ImGui::Spacing();
            ImGui::TextWrapped(
                "Loads arcdps addon DLLs when arcdps is unavailable. "
                "Provides minimal D3D11 + ImGui + arcdps API surface."
            );
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();
            ImGui::Text("Loaded extensions:");
            AddonManager::DispatchAboutInfo();
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    ImGui::End();
}

void ToggleVisibility() {
    s_visible = !s_visible;
}

bool IsVisible() {
    return s_visible;
}

bool HandleKeyInput(unsigned int msg, unsigned long long wparam, long long lparam) {
    (void)lparam;
    // Alt+Shift+T toggles the settings window (same keybind as arcdps options)
    if (msg == WM_KEYDOWN && wparam == 'T') {
        bool shift = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
        bool alt = (GetKeyState(VK_MENU) & 0x8000) != 0;
        if (shift && alt) {
            ToggleVisibility();
            return true;
        }
    }
    return false;
}

} // namespace SettingsWindow
