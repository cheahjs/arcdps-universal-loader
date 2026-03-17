#include "version_proxy.h"
#include "proxy_common.h"

namespace VersionProxy {

static HMODULE s_realVersion = nullptr;

// Function pointer types for version.dll exports
#define DECL_FP(name, ret, ...) using name##_t = ret(WINAPI*)(__VA_ARGS__); static name##_t s_real_##name = nullptr;

DECL_FP(GetFileVersionInfoA, BOOL, LPCSTR, DWORD, DWORD, LPVOID)
DECL_FP(GetFileVersionInfoW, BOOL, LPCWSTR, DWORD, DWORD, LPVOID)
DECL_FP(GetFileVersionInfoSizeA, DWORD, LPCSTR, LPDWORD)
DECL_FP(GetFileVersionInfoSizeW, DWORD, LPCWSTR, LPDWORD)
DECL_FP(VerQueryValueA, BOOL, LPCVOID, LPCSTR, LPVOID*, PUINT)
DECL_FP(VerQueryValueW, BOOL, LPCVOID, LPCWSTR, LPVOID*, PUINT)
DECL_FP(GetFileVersionInfoExA, BOOL, DWORD, LPCSTR, DWORD, DWORD, LPVOID)
DECL_FP(GetFileVersionInfoExW, BOOL, DWORD, LPCWSTR, DWORD, DWORD, LPVOID)
DECL_FP(GetFileVersionInfoSizeExA, DWORD, DWORD, LPCSTR, LPDWORD)
DECL_FP(GetFileVersionInfoSizeExW, DWORD, DWORD, LPCWSTR, LPDWORD)

#undef DECL_FP

bool Init() {
    s_realVersion = LoadRealSystemDll(L"version.dll");
    if (!s_realVersion) return false;

    #define RESOLVE(name) s_real_##name = (name##_t)GetProcAddress(s_realVersion, #name)
    RESOLVE(GetFileVersionInfoA);
    RESOLVE(GetFileVersionInfoW);
    RESOLVE(GetFileVersionInfoSizeA);
    RESOLVE(GetFileVersionInfoSizeW);
    RESOLVE(VerQueryValueA);
    RESOLVE(VerQueryValueW);
    RESOLVE(GetFileVersionInfoExA);
    RESOLVE(GetFileVersionInfoExW);
    RESOLVE(GetFileVersionInfoSizeExA);
    RESOLVE(GetFileVersionInfoSizeExW);
    #undef RESOLVE

    return true;
}

void Shutdown() {
    if (s_realVersion) {
        FreeLibrary(s_realVersion);
        s_realVersion = nullptr;
    }
}

} // namespace VersionProxy

extern "C" {

BOOL WINAPI GetFileVersionInfoA(LPCSTR lptstrFilename, DWORD dwHandle, DWORD dwLen, LPVOID lpData) {
    if (!VersionProxy::s_real_GetFileVersionInfoA) return FALSE;
    return VersionProxy::s_real_GetFileVersionInfoA(lptstrFilename, dwHandle, dwLen, lpData);
}

BOOL WINAPI GetFileVersionInfoW(LPCWSTR lptstrFilename, DWORD dwHandle, DWORD dwLen, LPVOID lpData) {
    if (!VersionProxy::s_real_GetFileVersionInfoW) return FALSE;
    return VersionProxy::s_real_GetFileVersionInfoW(lptstrFilename, dwHandle, dwLen, lpData);
}

DWORD WINAPI GetFileVersionInfoSizeA(LPCSTR lptstrFilename, LPDWORD lpdwHandle) {
    if (!VersionProxy::s_real_GetFileVersionInfoSizeA) return 0;
    return VersionProxy::s_real_GetFileVersionInfoSizeA(lptstrFilename, lpdwHandle);
}

DWORD WINAPI GetFileVersionInfoSizeW(LPCWSTR lptstrFilename, LPDWORD lpdwHandle) {
    if (!VersionProxy::s_real_GetFileVersionInfoSizeW) return 0;
    return VersionProxy::s_real_GetFileVersionInfoSizeW(lptstrFilename, lpdwHandle);
}

BOOL WINAPI VerQueryValueA(LPCVOID pBlock, LPCSTR lpSubBlock, LPVOID* lplpBuffer, PUINT puLen) {
    if (!VersionProxy::s_real_VerQueryValueA) return FALSE;
    return VersionProxy::s_real_VerQueryValueA(pBlock, lpSubBlock, lplpBuffer, puLen);
}

BOOL WINAPI VerQueryValueW(LPCVOID pBlock, LPCWSTR lpSubBlock, LPVOID* lplpBuffer, PUINT puLen) {
    if (!VersionProxy::s_real_VerQueryValueW) return FALSE;
    return VersionProxy::s_real_VerQueryValueW(pBlock, lpSubBlock, lplpBuffer, puLen);
}

BOOL WINAPI GetFileVersionInfoExA(DWORD dwFlags, LPCSTR lpwstrFilename, DWORD dwHandle, DWORD dwLen, LPVOID lpData) {
    if (!VersionProxy::s_real_GetFileVersionInfoExA) return FALSE;
    return VersionProxy::s_real_GetFileVersionInfoExA(dwFlags, lpwstrFilename, dwHandle, dwLen, lpData);
}

BOOL WINAPI GetFileVersionInfoExW(DWORD dwFlags, LPCWSTR lpwstrFilename, DWORD dwHandle, DWORD dwLen, LPVOID lpData) {
    if (!VersionProxy::s_real_GetFileVersionInfoExW) return FALSE;
    return VersionProxy::s_real_GetFileVersionInfoExW(dwFlags, lpwstrFilename, dwHandle, dwLen, lpData);
}

DWORD WINAPI GetFileVersionInfoSizeExA(DWORD dwFlags, LPCSTR lpwstrFilename, LPDWORD lpdwHandle) {
    if (!VersionProxy::s_real_GetFileVersionInfoSizeExA) return 0;
    return VersionProxy::s_real_GetFileVersionInfoSizeExA(dwFlags, lpwstrFilename, lpdwHandle);
}

DWORD WINAPI GetFileVersionInfoSizeExW(DWORD dwFlags, LPCWSTR lpwstrFilename, LPDWORD lpdwHandle) {
    if (!VersionProxy::s_real_GetFileVersionInfoSizeExW) return 0;
    return VersionProxy::s_real_GetFileVersionInfoSizeExW(dwFlags, lpwstrFilename, lpdwHandle);
}

} // extern "C"
