#pragma once

#include <windows.h>

namespace Singleton {
    bool TryAcquire();
    void Release();
    bool IsOwner();
}
