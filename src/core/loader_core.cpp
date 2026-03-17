#include "loader_core.h"
#include "detect/arcdps_detect.h"
#include "hook/wndproc_hook.h"
#include "imgui/imgui_impl.h"
#include "addons/addon_manager.h"
#include <chrono>

namespace LoaderCore {

static LoadMethod s_loadMethod = LoadMethod::Proxy;
static bool s_activated = false;
static bool s_disabled = false;
static bool s_firstPresent = false;
static std::chrono::steady_clock::time_point s_firstPresentTime;
static IDXGISwapChain* s_swapchain = nullptr;
static ID3D11Device* s_device = nullptr;
static ID3D11DeviceContext* s_context = nullptr;

static constexpr auto DETECTION_DELAY = std::chrono::seconds(5);

void SetLoadMethod(LoadMethod method) {
    s_loadMethod = method;
}

LoadMethod GetLoadMethod() {
    return s_loadMethod;
}

bool IsActivated() {
    return s_activated;
}

bool IsDisabled() {
    return s_disabled;
}

void OnPresent(IDXGISwapChain* swapchain) {
    if (s_disabled) return;

    // First Present: record timestamp and cache D3D11 objects
    if (!s_firstPresent) {
        s_firstPresent = true;
        s_firstPresentTime = std::chrono::steady_clock::now();
        s_swapchain = swapchain;

        // Get device and context from swapchain
        swapchain->GetDevice(__uuidof(ID3D11Device), (void**)&s_device);
        if (s_device) {
            s_device->GetImmediateContext(&s_context);
        }
        return;
    }

    // Waiting phase: check if detection delay has elapsed
    if (!s_activated) {
        auto now = std::chrono::steady_clock::now();
        if (now - s_firstPresentTime < DETECTION_DELAY) {
            return;
        }

        // Detection delay elapsed — check for arcdps
        if (ArcdpsDetect::IsArcdpsPresent()) {
            s_disabled = true;
            if (s_device) s_device->Release();
            if (s_context) s_context->Release();
            s_device = nullptr;
            s_context = nullptr;
            s_swapchain = nullptr;
            return;
        }

        Activate();
    }

    // Running phase
    ImGuiImpl::NewFrame();
    AddonManager::DispatchPresent();
    ImGuiImpl::Render();
}

void Activate() {
    if (s_activated || s_disabled) return;

    HWND hwnd = nullptr;
    if (s_swapchain) {
        DXGI_SWAP_CHAIN_DESC desc{};
        s_swapchain->GetDesc(&desc);
        hwnd = desc.OutputWindow;
    }

    // Init ImGui only if we own the context (not Nexus/gw2al)
    if (s_loadMethod == LoadMethod::Proxy || s_loadMethod == LoadMethod::GW2Load) {
        ImGuiImpl::Init(s_swapchain, s_device, s_context, hwnd);
    }

    // Install WndProc hook only if we manage our own hooks
    if (s_loadMethod == LoadMethod::Proxy || s_loadMethod == LoadMethod::GW2Load) {
        if (hwnd) {
            WndProcHook::Install(hwnd);
        }
    }

    // Get exe directory for addon scanning
    wchar_t exePath[MAX_PATH];
    GetModuleFileNameW(nullptr, exePath, MAX_PATH);
    std::wstring exeDir(exePath);
    auto lastSlash = exeDir.find_last_of(L'\\');
    if (lastSlash != std::wstring::npos) {
        exeDir = exeDir.substr(0, lastSlash + 1);
    }

    AddonManager::LoadAll(exeDir);
    s_activated = true;
}

void Shutdown() {
    if (!s_activated) return;

    AddonManager::UnloadAll();

    if (s_loadMethod == LoadMethod::Proxy || s_loadMethod == LoadMethod::GW2Load) {
        WndProcHook::Remove();
        ImGuiImpl::Shutdown();
    }

    if (s_device) {
        s_device->Release();
        s_device = nullptr;
    }
    if (s_context) {
        s_context->Release();
        s_context = nullptr;
    }
    s_swapchain = nullptr;
    s_activated = false;
}

} // namespace LoaderCore
