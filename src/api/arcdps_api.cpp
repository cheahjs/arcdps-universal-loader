#include "arcdps_api.h"
#include <cstdio>
#include <cstring>
#include <string>

// Config ini path (returned by e0)
static std::string s_configPath;
static bool s_configPathInit = false;

static const char* GetConfigPath() {
    if (!s_configPathInit) {
        char exePath[MAX_PATH];
        GetModuleFileNameA(nullptr, exePath, MAX_PATH);
        std::string dir(exePath);
        auto lastSlash = dir.find_last_of('\\');
        if (lastSlash != std::string::npos) {
            dir = dir.substr(0, lastSlash + 1);
        }
        s_configPath = dir + "addons\\arcdps\\arcdps.ini";
        s_configPathInit = true;
    }
    return s_configPath.c_str();
}

// Default color array for e5
static ImVec4 s_defaultColors[5] = {
    {1.0f, 1.0f, 1.0f, 1.0f}, // white
    {1.0f, 1.0f, 0.0f, 1.0f}, // yellow
    {1.0f, 0.5f, 0.0f, 1.0f}, // orange
    {1.0f, 0.0f, 0.0f, 1.0f}, // red
    {0.5f, 0.5f, 0.5f, 1.0f}, // gray
};

// Log to file
static void LogToFile(const char* str) {
    char exePath[MAX_PATH];
    GetModuleFileNameA(nullptr, exePath, MAX_PATH);
    std::string dir(exePath);
    auto lastSlash = dir.find_last_of('\\');
    if (lastSlash != std::string::npos) {
        dir = dir.substr(0, lastSlash + 1);
    }
    std::string logPath = dir + "addons\\arcdps\\arcdps_universal_loader.log";

    FILE* f = fopen(logPath.c_str(), "a");
    if (f) {
        fprintf(f, "%s\n", str);
        fclose(f);
    }
}

extern "C" {

// e0: returns path to config ini
void* e0() {
    return (void*)GetConfigPath();
}

// e3: log to file
void e3(const char* str) {
    if (str) {
        LogToFile(str);
    }
}

// e5: color settings
void e5(ImVec4** out) {
    if (out) {
        *out = s_defaultColors;
    }
}

// e6: UI settings bitmask (0 = defaults)
uint64_t e6() {
    return 0;
}

// e7: modifier keys bitmask
uint64_t e7() {
    uint64_t mods = 0;
    if (GetKeyState(VK_SHIFT) & 0x8000)   mods |= 1;
    if (GetKeyState(VK_CONTROL) & 0x8000)  mods |= 2;
    if (GetKeyState(VK_MENU) & 0x8000)     mods |= 4; // Alt
    return mods;
}

// e8: log to arcdps extension tab
void e8(const char* str) {
    if (str) {
        LogToFile(str);
    }
}

// e9: add combat event to another extension (no-op)
void e9(cbtevent* ev, uint32_t sig) {
    (void)ev;
    (void)sig;
}

// e10: add combat event area (no-op)
void e10(cbtevent* ev, uint32_t sig) {
    (void)ev;
    (void)sig;
}

} // extern "C"
