cmake_minimum_required(VERSION 3.20)
project(arcdps_universal_loader VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Proxy mode selection
set(PROXY_MODE "addon" CACHE STRING "Proxy mode: addon, d3d11, dxgi, version")
set_property(CACHE PROXY_MODE PROPERTY STRINGS addon d3d11 dxgi version)

# Output name based on proxy mode
if(PROXY_MODE STREQUAL "d3d11")
    set(OUTPUT_NAME "d3d11")
    set(DEF_FILE "${CMAKE_SOURCE_DIR}/def/d3d11.def")
elseif(PROXY_MODE STREQUAL "dxgi")
    set(OUTPUT_NAME "dxgi")
    set(DEF_FILE "${CMAKE_SOURCE_DIR}/def/dxgi.def")
elseif(PROXY_MODE STREQUAL "version")
    set(OUTPUT_NAME "version")
    set(DEF_FILE "${CMAKE_SOURCE_DIR}/def/version_proxy.def")
else()
    set(OUTPUT_NAME "arcdps_universal_loader")
    set(DEF_FILE "${CMAKE_SOURCE_DIR}/def/addon.def")
endif()

# Configure version resource
configure_file(
    "${CMAKE_SOURCE_DIR}/res/version.rc.in"
    "${CMAKE_BINARY_DIR}/version.rc"
    @ONLY
)

# Fetch ImGui v1.80
include(FetchContent)
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG        v1.80
)
FetchContent_MakeAvailable(imgui)

# ImGui library
add_library(imgui_lib STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_win32.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_dx11.cpp
)
target_include_directories(imgui_lib PUBLIC
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)
target_compile_definitions(imgui_lib PRIVATE
    IMGUI_IMPL_WIN32_DISABLE_GAMEPAD
)

# Main loader DLL — use fixed target name to avoid conflicts with link libraries
set(TARGET_NAME "arcdps_loader")

add_library(${TARGET_NAME} SHARED
    src/dllmain.cpp
    src/core/singleton.cpp
    src/core/loader_core.cpp
    src/detect/arcdps_detect.cpp
    src/hook/d3d11_hook.cpp
    src/hook/wndproc_hook.cpp
    src/imgui/imgui_impl.cpp
    src/addons/addon_scanner.cpp
    src/addons/addon_instance.cpp
    src/addons/addon_manager.cpp
    src/api/arcdps_api.cpp
    src/entry/entry_nexus.cpp
    src/entry/entry_gw2al.cpp
    src/entry/entry_gw2load.cpp
    src/entry/entry_proxy.cpp
    src/proxy/d3d11_proxy.cpp
    src/proxy/dxgi_proxy.cpp
    src/proxy/version_proxy.cpp
    ${DEF_FILE}
    ${CMAKE_BINARY_DIR}/version.rc
)

set_target_properties(${TARGET_NAME} PROPERTIES
    OUTPUT_NAME "${OUTPUT_NAME}"
    PREFIX ""
    SUFFIX ".dll"
)

target_include_directories(${TARGET_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(${TARGET_NAME} PRIVATE
    imgui_lib
    d3d11
    dxgi
    d3dcompiler
    dwmapi
    xinput
    version
)

target_compile_definitions(${TARGET_NAME} PRIVATE
    UNICODE
    _UNICODE
    WIN32_LEAN_AND_MEAN
    NOMINMAX
)

if(MINGW)
    target_link_options(${TARGET_NAME} PRIVATE
        -static-libgcc
        -static-libstdc++
        -static
    )
endif()
