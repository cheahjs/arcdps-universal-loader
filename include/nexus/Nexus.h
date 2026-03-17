#pragma once

#include <stdint.h>
#include <windows.h>

/* Minimal Nexus addon API — only what we need to be a Nexus addon */

typedef int32_t NEXUS_SIGNED;
typedef uint32_t NEXUS_UNSIGNED;

enum class EAddonFlags : int32_t {
    None = 0,
    IsVolatile = 1,
    DisableHotloading = 2,
};

enum class EUpdateProvider : int32_t {
    None = 0,
    Raidcore = 1,
    GitHub = 2,
    Direct = 3,
};

struct AddonVersion {
    NEXUS_SIGNED Major;
    NEXUS_SIGNED Minor;
    NEXUS_SIGNED Build;
    NEXUS_SIGNED Revision;
};

/* GUI render callback */
typedef void (*GUI_RENDER)();
typedef void (*GUI_ADDRENDER)(GUI_RENDER callback);
typedef void (*GUI_REMRENDER)(GUI_RENDER callback);

/* Event callbacks */
typedef void (*EVENTS_RAISE)(const char* identifier, void* eventData);
typedef void (*EVENTS_SUBSCRIBE)(const char* identifier, void (*callback)(void* eventData));

/* Logging */
typedef void (*LOG)(int logLevel, const char* str);

/* Nexus addon API table — provided to addon via Load callback */
struct AddonAPI {
    /* Renderer */
    GUI_ADDRENDER RegisterRender;
    GUI_REMRENDER DeregisterRender;

    /* Events */
    EVENTS_RAISE RaiseEvent;
    EVENTS_SUBSCRIBE SubscribeEvent;

    /* Logging (log level: 0=off, 1=critical, 2=warning, 3=info, 4=debug, 5=trace) */
    LOG Log;

    /* ImGui context (for direct use) */
    void* ImguiContext;
    void* ImguiMalloc;
    void* ImguiFree;
};

/* Load/Unload signatures */
typedef void (*ADDON_LOAD)(AddonAPI* api);
typedef void (*ADDON_UNLOAD)();

/* Nexus addon definition */
struct AddonDefinition {
    NEXUS_SIGNED Signature;         /* unique addon signature / raidcore id */
    NEXUS_SIGNED APIVersion;        /* nexus api version (use -1 for unversioned) */
    const char* Name;
    AddonVersion Version;
    const char* Author;
    const char* Description;
    ADDON_LOAD Load;
    ADDON_UNLOAD Unload;
    EAddonFlags Flags;
    EUpdateProvider Provider;
    const char* UpdateLink;
};

/* The export that Nexus looks for */
extern "C" __declspec(dllexport) AddonDefinition* GetAddonDef();
