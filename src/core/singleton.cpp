#include "singleton.h"

namespace Singleton {

static HANDLE s_mutex = nullptr;
static bool s_owner = false;

bool TryAcquire() {
    s_mutex = CreateMutexW(nullptr, TRUE, L"Global\\ArcdpsUniversalLoader");
    if (s_mutex == nullptr) {
        return false;
    }
    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        CloseHandle(s_mutex);
        s_mutex = nullptr;
        s_owner = false;
        return false;
    }
    s_owner = true;
    return true;
}

void Release() {
    if (s_mutex) {
        ReleaseMutex(s_mutex);
        CloseHandle(s_mutex);
        s_mutex = nullptr;
    }
    s_owner = false;
}

bool IsOwner() {
    return s_owner;
}

} // namespace Singleton
