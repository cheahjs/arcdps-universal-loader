#include "imgui_impl.h"
#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>
#include <string>

namespace ImGuiImpl {

static ImGuiContext* s_context = nullptr;
static bool s_ownContext = false;
static ID3D11RenderTargetView* s_rtv = nullptr;
static IDXGISwapChain* s_swapchain = nullptr;
static ID3D11Device* s_device = nullptr;
static ID3D11DeviceContext* s_deviceContext = nullptr;

static void CreateRenderTarget() {
    if (!s_swapchain || !s_device) return;
    ID3D11Texture2D* backBuffer = nullptr;
    s_swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);
    if (backBuffer) {
        s_device->CreateRenderTargetView(backBuffer, nullptr, &s_rtv);
        backBuffer->Release();
    }
}

static std::string GetImguiIniPath() {
    // Use same ini path as arcdps for window position persistence
    char exePath[MAX_PATH];
    GetModuleFileNameA(nullptr, exePath, MAX_PATH);
    std::string dir(exePath);
    auto lastSlash = dir.find_last_of('\\');
    if (lastSlash != std::string::npos) {
        dir = dir.substr(0, lastSlash + 1);
    }
    return dir + "addons\\arcdps\\arcdps_imgui.ini";
}

static std::string s_iniPath;

void Init(IDXGISwapChain* swapchain, ID3D11Device* device, ID3D11DeviceContext* context, HWND hwnd) {
    s_swapchain = swapchain;
    s_device = device;
    s_deviceContext = context;

    s_context = ImGui::CreateContext();
    s_ownContext = true;

    ImGuiIO& io = ImGui::GetIO();
    s_iniPath = GetImguiIniPath();
    io.IniFilename = s_iniPath.c_str();

    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(device, context);

    CreateRenderTarget();
}

void Shutdown() {
    if (s_rtv) {
        s_rtv->Release();
        s_rtv = nullptr;
    }
    if (s_ownContext) {
        ImGui_ImplDX11_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext(s_context);
    }
    s_context = nullptr;
    s_ownContext = false;
}

void NewFrame() {
    if (!s_context) return;
    if (s_ownContext) {
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
    }
    ImGui::NewFrame();
}

void Render() {
    if (!s_context) return;
    ImGui::Render();
    if (s_ownContext && s_rtv && s_deviceContext) {
        s_deviceContext->OMSetRenderTargets(1, &s_rtv, nullptr);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    }
}

void* GetContext() {
    return s_context;
}

void SetExternalContext(void* ctx) {
    s_context = (ImGuiContext*)ctx;
    s_ownContext = false;
    ImGui::SetCurrentContext(s_context);
}

} // namespace ImGuiImpl
