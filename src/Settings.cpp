#include "Settings.hpp"
#include <string>

Settings::Settings() {
    colorGpu = RGB(30, 144, 255); // DodgerBlue
    colorTemp = RGB(0, 255, 0);   // Green
    colorMem = RGB(255, 165, 0);  // Orange
}

bool Settings::IsAutoRunEnabled() {
    HKEY hKey;
    LONG lRes = RegOpenKeyExA(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_READ, &hKey);
    if (lRes != ERROR_SUCCESS) return false;

    char value[1024];
    DWORD size = sizeof(value);
    lRes = RegQueryValueExA(hKey, "NVBar", NULL, NULL, (LPBYTE)value, &size);
    RegCloseKey(hKey);
    return (lRes == ERROR_SUCCESS);
}

void Settings::SetAutoRun(bool enable) {
    HKEY hKey;
    LONG lRes = RegOpenKeyExA(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_WRITE, &hKey);
    if (lRes != ERROR_SUCCESS) return;

    if (enable) {
        char path[MAX_PATH];
        GetModuleFileNameA(NULL, path, MAX_PATH);
        std::string cmd = "\"" + std::string(path) + "\"";
        RegSetValueExA(hKey, "NVBar", 0, REG_SZ, (const BYTE*)cmd.c_str(), cmd.length() + 1);
    } else {
        RegDeleteValueA(hKey, "NVBar");
    }
    RegCloseKey(hKey);
}
