#include "d3d11_hook.h"
#include "core/loader_core.h"
#include <cstring>

namespace D3D11Hook {

static void** s_vtable = nullptr;
static void* s_originalPresent = nullptr;
static void* s_originalResizeBuffers = nullptr;

using PresentFn = HRESULT(STDMETHODCALLTYPE*)(IDXGISwapChain*, UINT, UINT);
using ResizeBuffersFn = HRESULT(STDMETHODCALLTYPE*)(IDXGISwapChain*, UINT, UINT, UINT, DXGI_FORMAT, UINT);

static HRESULT STDMETHODCALLTYPE HookedPresent(IDXGISwapChain* swapchain, UINT syncInterval, UINT flags) {
    LoaderCore::OnPresent(swapchain);
    return ((PresentFn)s_originalPresent)(swapchain, syncInterval, flags);
}

static HRESULT STDMETHODCALLTYPE HookedResizeBuffers(IDXGISwapChain* swapchain, UINT bufferCount,
                                                       UINT width, UINT height, DXGI_FORMAT format, UINT flags) {
    // TODO: handle resize (re-create render target view)
    return ((ResizeBuffersFn)s_originalResizeBuffers)(swapchain, bufferCount, width, height, format, flags);
}

void Install(IDXGISwapChain* swapchain) {
    if (s_vtable) return;

    // Get vtable from swapchain COM object
    s_vtable = *(void***)swapchain;

    // Save originals
    s_originalPresent = s_vtable[8];
    s_originalResizeBuffers = s_vtable[13];

    // Patch vtable
    DWORD oldProtect;
    VirtualProtect(&s_vtable[8], sizeof(void*), PAGE_READWRITE, &oldProtect);
    s_vtable[8] = (void*)&HookedPresent;
    VirtualProtect(&s_vtable[8], sizeof(void*), oldProtect, &oldProtect);

    VirtualProtect(&s_vtable[13], sizeof(void*), PAGE_READWRITE, &oldProtect);
    s_vtable[13] = (void*)&HookedResizeBuffers;
    VirtualProtect(&s_vtable[13], sizeof(void*), oldProtect, &oldProtect);
}

void Remove() {
    if (!s_vtable) return;

    DWORD oldProtect;
    VirtualProtect(&s_vtable[8], sizeof(void*), PAGE_READWRITE, &oldProtect);
    s_vtable[8] = s_originalPresent;
    VirtualProtect(&s_vtable[8], sizeof(void*), oldProtect, &oldProtect);

    VirtualProtect(&s_vtable[13], sizeof(void*), PAGE_READWRITE, &oldProtect);
    s_vtable[13] = s_originalResizeBuffers;
    VirtualProtect(&s_vtable[13], sizeof(void*), oldProtect, &oldProtect);

    s_vtable = nullptr;
    s_originalPresent = nullptr;
    s_originalResizeBuffers = nullptr;
}

} // namespace D3D11Hook
