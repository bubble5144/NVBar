#pragma once
#include <windows.h>

class Settings {
public:
    Settings();

    bool IsAutoRunEnabled();
    void SetAutoRun(bool enable);

    COLORREF colorGpu;
    COLORREF colorTemp;
    COLORREF colorMem;
};
