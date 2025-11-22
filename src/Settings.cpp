#include "Settings.hpp"
#include <string>

// 注册表路径常量
#define REG_KEY_PATH "Software\\NVBar"
#define REG_VALUE_COLOR_GPU "ColorGpu"
#define REG_VALUE_COLOR_TEMP "ColorTemp"
#define REG_VALUE_COLOR_MEM "ColorMem"

Settings::Settings() {
    // 默认颜色
    colorGpu = RGB(30, 144, 255); // DodgerBlue
    colorTemp = RGB(0, 255, 0);   // Green
    colorMem = RGB(255, 165, 0);  // Orange
    
    // 从注册表加载保存的颜色设置
    LoadColors();
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
        RegSetValueExA(hKey, "NVBar", 0, REG_SZ, (const BYTE*)cmd.c_str(), (DWORD)(cmd.length() + 1));
    } else {
        RegDeleteValueA(hKey, "NVBar");
    }
    RegCloseKey(hKey);
}

void Settings::LoadColors() {
    HKEY hKey;
    LONG lRes = RegOpenKeyExA(HKEY_CURRENT_USER, REG_KEY_PATH, 0, KEY_READ, &hKey);
    if (lRes != ERROR_SUCCESS) {
        // 如果键不存在，使用默认值
        return;
    }

    DWORD value;
    DWORD size = sizeof(DWORD);
    DWORD type;

    // 加载GPU颜色
    lRes = RegQueryValueExA(hKey, REG_VALUE_COLOR_GPU, NULL, &type, (LPBYTE)&value, &size);
    if (lRes == ERROR_SUCCESS && type == REG_DWORD) {
        colorGpu = (COLORREF)value;
    }

    // 加载温度颜色
    lRes = RegQueryValueExA(hKey, REG_VALUE_COLOR_TEMP, NULL, &type, (LPBYTE)&value, &size);
    if (lRes == ERROR_SUCCESS && type == REG_DWORD) {
        colorTemp = (COLORREF)value;
    }

    // 加载内存颜色
    lRes = RegQueryValueExA(hKey, REG_VALUE_COLOR_MEM, NULL, &type, (LPBYTE)&value, &size);
    if (lRes == ERROR_SUCCESS && type == REG_DWORD) {
        colorMem = (COLORREF)value;
    }

    RegCloseKey(hKey);
}

void Settings::SaveColors() {
    HKEY hKey;
    LONG lRes = RegCreateKeyExA(HKEY_CURRENT_USER, REG_KEY_PATH, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, NULL);
    if (lRes != ERROR_SUCCESS) return;

    DWORD value;

    // 保存GPU颜色
    value = (DWORD)colorGpu;
    RegSetValueExA(hKey, REG_VALUE_COLOR_GPU, 0, REG_DWORD, (const BYTE*)&value, sizeof(DWORD));

    // 保存温度颜色
    value = (DWORD)colorTemp;
    RegSetValueExA(hKey, REG_VALUE_COLOR_TEMP, 0, REG_DWORD, (const BYTE*)&value, sizeof(DWORD));

    // 保存内存颜色
    value = (DWORD)colorMem;
    RegSetValueExA(hKey, REG_VALUE_COLOR_MEM, 0, REG_DWORD, (const BYTE*)&value, sizeof(DWORD));

    RegCloseKey(hKey);
}
