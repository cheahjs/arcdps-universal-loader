#pragma once

#include <windows.h>
#include <string>
#include <vector>

namespace AddonScanner {
    std::vector<std::wstring> ScanForAddons(const std::wstring& directory);
}
