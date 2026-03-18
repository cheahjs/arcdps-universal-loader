#include "addon_manager.h"
#include "addon_scanner.h"
#include "addon_instance.h"
#include "imgui/imgui_impl.h"
#include <imgui.h>
#include <vector>
#include <mutex>

namespace AddonManager {

static std::vector<AddonInstance> s_addons;
static std::mutex s_mutex;

static HMODULE GetOurModule() {
    HMODULE hm = nullptr;
    GetModuleHandleExW(
        GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
        (LPCWSTR)&GetOurModule,
        &hm
    );
    return hm;
}

void LoadAll(const std::wstring& baseDir) {
    std::lock_guard<std::mutex> lock(s_mutex);

    auto paths = AddonScanner::ScanForAddons(baseDir);
    HMODULE ourModule = GetOurModule();
    void* imguiCtx = ImGuiImpl::GetContext();

    for (const auto& path : paths) {
        AddonInstance addon;
        if (addon.Load(path, imguiCtx, ourModule)) {
            s_addons.push_back(std::move(addon));
        }
    }
}

void UnloadAll() {
    std::lock_guard<std::mutex> lock(s_mutex);
    for (auto& addon : s_addons) {
        addon.Unload();
    }
    s_addons.clear();
}

void DispatchPresent() {
    std::lock_guard<std::mutex> lock(s_mutex);
    for (auto& addon : s_addons) {
        addon.CallImgui(1); // 1 = not charsel or loading
    }
}

bool DispatchWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    std::lock_guard<std::mutex> lock(s_mutex);

    // Dispatch wnd_nofilter to all addons (informational)
    for (auto& addon : s_addons) {
        addon.CallWndProcNoFilter(hwnd, msg, wparam, lparam);
    }

    // Dispatch wnd_filter — if any addon returns non-zero, message is consumed
    for (auto& addon : s_addons) {
        if (addon.CallWndProcFilter(hwnd, msg, wparam, lparam)) {
            return true;
        }
    }

    return false;
}

void DispatchOptionsEnd() {
    std::lock_guard<std::mutex> lock(s_mutex);
    for (auto& addon : s_addons) {
        addon.CallOptionsEnd();
    }
}

void DispatchOptionsWindows(const char* windowName) {
    std::lock_guard<std::mutex> lock(s_mutex);
    for (auto& addon : s_addons) {
        addon.CallOptionsWindows(windowName);
    }
}

void DispatchExtensionTabs() {
    std::lock_guard<std::mutex> lock(s_mutex);
    for (auto& addon : s_addons) {
        if (!addon.IsLoaded()) continue;
        const char* name = addon.GetName();
        if (ImGui::BeginTabItem(name)) {
            addon.CallOptionsEnd();
            ImGui::EndTabItem();
        }
    }
}

void DispatchAboutInfo() {
    std::lock_guard<std::mutex> lock(s_mutex);
    if (s_addons.empty()) {
        ImGui::TextDisabled("No extensions loaded");
        return;
    }
    for (auto& addon : s_addons) {
        if (!addon.IsLoaded()) continue;
        ImGui::BulletText("%s", addon.GetName());
    }
}

} // namespace AddonManager
