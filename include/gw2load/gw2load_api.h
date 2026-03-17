#pragma once

#include <stdint.h>
#include <windows.h>

/* GW2Load addon API — from api.h */

struct GW2Load_Version_t {
    uint16_t major;
    uint16_t minor;
    uint16_t patch;
    uint16_t build;
};

/* Current API version constant */
constexpr uint32_t GW2Load_CurrentAddonAPIVersion = 1;

/* Callback types */
typedef void (*GW2Load_PresentCallback_t)(void* swapchain);
typedef void (*GW2Load_ResizeCallback_t)(void* swapchain, uint32_t width, uint32_t height);
typedef void (*GW2Load_WndProcCallback_t)(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam, bool* handled);

/* Export signatures */
extern "C" {
    __declspec(dllexport) uint32_t GW2Load_GetAddonAPIVersion();
    __declspec(dllexport) void GW2Load_OnLoad(void* api);
    __declspec(dllexport) void GW2Load_OnClose();
}
