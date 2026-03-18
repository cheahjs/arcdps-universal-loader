#pragma once

namespace SettingsWindow {
    void Render();
    void ToggleVisibility();
    bool IsVisible();

    // Called from WndProc to check for Alt+Shift+T toggle
    bool HandleKeyInput(unsigned int msg, unsigned long long wparam, long long lparam);
}
