#pragma once

#include <windows.h>
#include <arcdps/arcdps_structs.h>
#include <imgui.h>

extern "C" {
    __declspec(dllexport) void* e0();
    __declspec(dllexport) void e3(const char* str);
    __declspec(dllexport) void e5(ImVec4** out);
    __declspec(dllexport) uint64_t e6();
    __declspec(dllexport) uint64_t e7();
    __declspec(dllexport) void e8(const char* str);
    __declspec(dllexport) void e9(cbtevent* ev, uint32_t sig);
    __declspec(dllexport) void e10(cbtevent* ev, uint32_t sig);
}
