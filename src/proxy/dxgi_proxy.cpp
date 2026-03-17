#include "dxgi_proxy.h"
#include "proxy_common.h"
#include <dxgi.h>

namespace DXGIProxy {

static HMODULE s_realDXGI = nullptr;

using CreateDXGIFactory_t = HRESULT(WINAPI*)(REFIID, void**);
using CreateDXGIFactory1_t = HRESULT(WINAPI*)(REFIID, void**);
using CreateDXGIFactory2_t = HRESULT(WINAPI*)(UINT, REFIID, void**);

static CreateDXGIFactory_t s_realCreateDXGIFactory = nullptr;
static CreateDXGIFactory1_t s_realCreateDXGIFactory1 = nullptr;
static CreateDXGIFactory2_t s_realCreateDXGIFactory2 = nullptr;

bool Init() {
    s_realDXGI = LoadRealSystemDll(L"dxgi.dll");
    if (!s_realDXGI) return false;

    s_realCreateDXGIFactory = (CreateDXGIFactory_t)GetProcAddress(s_realDXGI, "CreateDXGIFactory");
    s_realCreateDXGIFactory1 = (CreateDXGIFactory1_t)GetProcAddress(s_realDXGI, "CreateDXGIFactory1");
    s_realCreateDXGIFactory2 = (CreateDXGIFactory2_t)GetProcAddress(s_realDXGI, "CreateDXGIFactory2");

    return true;
}

void Shutdown() {
    if (s_realDXGI) {
        FreeLibrary(s_realDXGI);
        s_realDXGI = nullptr;
    }
}

} // namespace DXGIProxy

extern "C" {

HRESULT WINAPI CreateDXGIFactory(REFIID riid, void** ppFactory) {
    if (!DXGIProxy::s_realCreateDXGIFactory) return E_FAIL;
    return DXGIProxy::s_realCreateDXGIFactory(riid, ppFactory);
}

HRESULT WINAPI CreateDXGIFactory1(REFIID riid, void** ppFactory) {
    if (!DXGIProxy::s_realCreateDXGIFactory1) return E_FAIL;
    return DXGIProxy::s_realCreateDXGIFactory1(riid, ppFactory);
}

HRESULT WINAPI CreateDXGIFactory2(UINT flags, REFIID riid, void** ppFactory) {
    if (!DXGIProxy::s_realCreateDXGIFactory2) return E_FAIL;
    return DXGIProxy::s_realCreateDXGIFactory2(flags, riid, ppFactory);
}

} // extern "C"
