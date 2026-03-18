#pragma once

#include <windows.h>
#include <arcdps/arcdps_structs.h>
#include <imgui.h>

extern "C" {
    void* e0();
    void e3(const char* str);
    void e5(ImVec4** out);
    uint64_t e6();
    uint64_t e7();
    void e8(const char* str);
    void e9(cbtevent* ev, uint32_t sig);
    void e10(cbtevent* ev, uint32_t sig);
}
