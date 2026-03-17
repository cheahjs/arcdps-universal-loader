#include "addon_scanner.h"

namespace AddonScanner {

static HMODULE GetOurModule() {
    HMODULE hm = nullptr;
    GetModuleHandleExW(
        GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
        (LPCWSTR)&GetOurModule,
        &hm
    );
    return hm;
}

std::vector<std::wstring> ScanForAddons(const std::wstring& directory) {
    std::vector<std::wstring> result;
    HMODULE ourModule = GetOurModule();

    std::wstring searchPath = directory + L"*.dll";

    WIN32_FIND_DATAW fd;
    HANDLE hFind = FindFirstFileW(searchPath.c_str(), &fd);
    if (hFind == INVALID_HANDLE_VALUE) {
        return result;
    }

    do {
        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) continue;

        std::wstring fullPath = directory + fd.cFileName;

        // Skip known non-addon DLLs
        std::wstring name(fd.cFileName);
        for (auto& c : name) c = towlower(c);
        if (name == L"d3d11.dll" || name == L"dxgi.dll" || name == L"version.dll" ||
            name == L"d3dcompiler_47.dll" || name == L"bin64input.dll") {
            continue;
        }

        // Try loading without executing to check for get_init_addr
        HMODULE hTest = LoadLibraryExW(fullPath.c_str(), nullptr, DONT_RESOLVE_DLL_REFERENCES);
        if (!hTest) continue;
        if (hTest == ourModule) {
            FreeLibrary(hTest);
            continue;
        }

        auto proc = GetProcAddress(hTest, "get_init_addr");
        FreeLibrary(hTest);

        if (proc) {
            result.push_back(fullPath);
        }
    } while (FindNextFileW(hFind, &fd));

    FindClose(hFind);
    return result;
}

} // namespace AddonScanner
