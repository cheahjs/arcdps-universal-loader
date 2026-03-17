#pragma once

#include <stdint.h>
#include <windows.h>

/* Minimal gw2-addon-loader API */

enum gw2al_api_ret {
    GW2AL_OK = 0,
    GW2AL_FAIL = 1,
};

typedef uint64_t gw2al_hashed_name;

/* Addon description */
struct gw2al_addon_dsc {
    const wchar_t* name;
    const wchar_t* description;
    uint32_t majorVer;
    uint32_t minorVer;
    uint32_t revision;
    uint32_t build;
    /* null-terminated array of dependency entries */
    struct gw2al_addon_dsc* dependList;
};

/* Function pointer types */
typedef void* (*gw2al_query_function_t)(gw2al_hashed_name name);
typedef gw2al_api_ret (*gw2al_register_function_t)(void* func, gw2al_hashed_name name);
typedef gw2al_api_ret (*gw2al_subscribe_event_t)(gw2al_hashed_name name, void* callback);
typedef gw2al_api_ret (*gw2al_unsubscribe_event_t)(gw2al_hashed_name name, void* callback);
typedef gw2al_api_ret (*gw2al_trigger_event_t)(gw2al_hashed_name name, void* data);

/* Core vtable — provided by gw2-addon-loader */
struct gw2al_core_vtable {
    gw2al_query_function_t query_function;
    gw2al_register_function_t register_function;
    gw2al_subscribe_event_t subscribe_event;
    gw2al_unsubscribe_event_t unsubscribe_event;
    gw2al_trigger_event_t trigger_event;
    /* hash utility */
    gw2al_hashed_name (*hash_name)(const wchar_t* name);
    /* logging */
    void (*log_text)(int severity, const wchar_t* source, const wchar_t* text);
};

/* Hash helper macro — we compute at runtime */
inline gw2al_hashed_name gw2al_hash(gw2al_core_vtable* core, const wchar_t* name) {
    return core->hash_name(name);
}

/* Addon export signatures */
extern "C" {
    __declspec(dllexport) gw2al_addon_dsc* gw2addon_get_description();
    __declspec(dllexport) gw2al_api_ret gw2addon_load(gw2al_core_vtable* core);
    __declspec(dllexport) gw2al_api_ret gw2addon_unload(gw2al_core_vtable* core);
}
