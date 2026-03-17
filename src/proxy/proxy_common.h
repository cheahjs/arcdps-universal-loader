#pragma once

#include <windows.h>
#include <string>

inline HMODULE LoadRealSystemDll(const wchar_t* dllName) {
    wchar_t systemDir[MAX_PATH];
    GetSystemDirectoryW(systemDir, MAX_PATH);
    std::wstring path = std::wstring(systemDir) + L"\\" + dllName;
    return LoadLibraryW(path.c_str());
}
