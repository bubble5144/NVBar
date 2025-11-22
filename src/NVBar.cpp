#include <windows.h>
#include <string>
#include <sstream>
#include <vector>
#include "GpuMonitor.hpp"
#include "Settings.hpp"

#define IDM_EXIT 1001
#define IDM_AUTORUN 1002
#define IDM_COLOR_GPU_RED 2001
#define IDM_COLOR_GPU_GREEN 2002
#define IDM_COLOR_GPU_BLUE 2003
#define IDM_COLOR_GPU_WHITE 2004
#define IDM_COLOR_TEMP_RED 2005
#define IDM_COLOR_TEMP_GREEN 2006
#define IDM_COLOR_TEMP_BLUE 2007
#define IDM_COLOR_TEMP_WHITE 2008
#define IDM_COLOR_MEM_RED 2009
#define IDM_COLOR_MEM_GREEN 2010
#define IDM_COLOR_MEM_BLUE 2011
#define IDM_COLOR_MEM_WHITE 2012

struct AppContext {
    GpuMonitor monitor;
    Settings settings;
};

void UpdateLayeredWindowContent(HWND hwnd, AppContext* ctx) {
    RECT rc;
    GetWindowRect(hwnd, &rc);
    int width = rc.right - rc.left;
    int height = rc.bottom - rc.top;

    HDC hdcScreen = GetDC(NULL);
    HDC hdcMem = CreateCompatibleDC(hdcScreen);
    
    // Create 32-bit bitmap for alpha channel
    BITMAPINFO bmi = {0};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = height; // Top-down
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    void* pBits = NULL;
    HBITMAP hBitmap = CreateDIBSection(hdcScreen, &bmi, DIB_RGB_COLORS, &pBits, NULL, 0);
    HBITMAP hOldBitmap = (HBITMAP)SelectObject(hdcMem, hBitmap);

    // Initialize with almost-transparent background (Alpha=1)
    DWORD* pixels = (DWORD*)pBits;
    int pixelCount = width * height;
    for (int i = 0; i < pixelCount; i++) {
        pixels[i] = 0x01000000; 
    }

    // Draw Text
    HFONT hFont = CreateFontA(18, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "Segoe UI");
    HFONT hOldFont = (HFONT)SelectObject(hdcMem, hFont);
    SetBkMode(hdcMem, TRANSPARENT);

    RECT rect = {5, 0, width, height}; // Padding left 5
    
    auto drawPart = [&](std::string text, COLORREF color) {
        SetTextColor(hdcMem, color);
        RECT measureRect = rect;
        DrawTextA(hdcMem, text.c_str(), -1, &rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);
        DrawTextA(hdcMem, text.c_str(), -1, &measureRect, DT_CALCRECT | DT_SINGLELINE);
        rect.left += (measureRect.right - measureRect.left);
    };

    drawPart("GPU: ", RGB(220, 220, 220));
    drawPart(std::to_string(ctx->monitor.GetGpuUtil()) + "% ", ctx->settings.colorGpu);
    
    drawPart(" TEMP: ", RGB(220, 220, 220));
    drawPart(std::to_string(ctx->monitor.GetTemp()) + "C ", ctx->settings.colorTemp);

    drawPart(" MEM: ", RGB(220, 220, 220));
    drawPart(std::to_string(ctx->monitor.GetMemUtil()) + "%", ctx->settings.colorMem);

    // Post-process alpha channel
    for (int i = 0; i < pixelCount; i++) {
        if ((pixels[i] & 0x00FFFFFF) != 0) {
            pixels[i] |= 0xFF000000; // Set Alpha to 255
        }
    }

    // Update Layered Window
    POINT ptSrc = {0, 0};
    SIZE sizeWnd = {width, height};
    BLENDFUNCTION blend = {0};
    blend.BlendOp = AC_SRC_OVER;
    blend.SourceConstantAlpha = 255;
    blend.AlphaFormat = AC_SRC_ALPHA;

    UpdateLayeredWindow(hwnd, hdcScreen, NULL, &sizeWnd, hdcMem, &ptSrc, 0, &blend, ULW_ALPHA);

    SelectObject(hdcMem, hOldFont);
    DeleteObject(hFont);
    SelectObject(hdcMem, hOldBitmap);
    DeleteObject(hBitmap);
    DeleteDC(hdcMem);
    ReleaseDC(NULL, hdcScreen);
}

void AdjustWindowSize(HWND hwnd, AppContext* ctx) {
    HDC hdc = GetDC(hwnd);
    HFONT hFont = CreateFontA(18, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "Segoe UI");
    HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);

    std::string s = "GPU: " + std::to_string(ctx->monitor.GetGpuUtil()) + "%  TEMP: " + std::to_string(ctx->monitor.GetTemp()) + "C  MEM: " + std::to_string(ctx->monitor.GetMemUtil()) + "%";
    RECT rect = {0, 0, 0, 0};
    DrawTextA(hdc, s.c_str(), -1, &rect, DT_CALCRECT | DT_SINGLELINE);

    SelectObject(hdc, hOldFont);
    DeleteObject(hFont);
    ReleaseDC(hwnd, hdc);

    int width = rect.right - rect.left + 20; // +20 padding
    
    RECT rcWindow;
    GetWindowRect(hwnd, &rcWindow);
    if ((rcWindow.right - rcWindow.left) != width) {
        SetWindowPos(hwnd, 0, 0, 0, width, rcWindow.bottom - rcWindow.top, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
    }
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    AppContext* ctx = (AppContext*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

    switch (uMsg) {
    case WM_CREATE:
        {
            CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;
            ctx = (AppContext*)pCreate->lpCreateParams;
            SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)ctx);
            SetTimer(hwnd, 1, 1000, NULL);
        }
        return 0;

    case WM_TIMER:
        if (ctx) {
            ctx->monitor.Update();
            AdjustWindowSize(hwnd, ctx);
            UpdateLayeredWindowContent(hwnd, ctx);
        }
        return 0;

    case WM_PAINT:
        ValidateRect(hwnd, NULL);
        return 0;

    case WM_LBUTTONDOWN:
        ReleaseCapture();
        SendMessage(hwnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
        return 0;

    case WM_COMMAND:
        if (ctx) {
            int id = LOWORD(wParam);
            if (id == IDM_EXIT) {
                DestroyWindow(hwnd);
            } else if (id == IDM_AUTORUN) {
                bool enabled = ctx->settings.IsAutoRunEnabled();
                ctx->settings.SetAutoRun(!enabled);
            } 
            // Color Handlers
            else if (id == IDM_COLOR_GPU_RED) ctx->settings.colorGpu = RGB(255, 0, 0);
            else if (id == IDM_COLOR_GPU_GREEN) ctx->settings.colorGpu = RGB(0, 255, 0);
            else if (id == IDM_COLOR_GPU_BLUE) ctx->settings.colorGpu = RGB(30, 144, 255);
            else if (id == IDM_COLOR_GPU_WHITE) ctx->settings.colorGpu = RGB(255, 255, 255);
            else if (id == IDM_COLOR_TEMP_RED) ctx->settings.colorTemp = RGB(255, 0, 0);
            else if (id == IDM_COLOR_TEMP_GREEN) ctx->settings.colorTemp = RGB(0, 255, 0);
            else if (id == IDM_COLOR_TEMP_BLUE) ctx->settings.colorTemp = RGB(30, 144, 255);
            else if (id == IDM_COLOR_TEMP_WHITE) ctx->settings.colorTemp = RGB(255, 255, 255);
            else if (id == IDM_COLOR_MEM_RED) ctx->settings.colorMem = RGB(255, 0, 0);
            else if (id == IDM_COLOR_MEM_GREEN) ctx->settings.colorMem = RGB(0, 255, 0);
            else if (id == IDM_COLOR_MEM_BLUE) ctx->settings.colorMem = RGB(30, 144, 255);
            else if (id == IDM_COLOR_MEM_WHITE) ctx->settings.colorMem = RGB(255, 255, 255);

            if (id >= 2000) { 
                UpdateLayeredWindowContent(hwnd, ctx);
            }
        }
        return 0;

    case WM_RBUTTONUP:
        if (ctx) {
            POINT pt;
            GetCursorPos(&pt);
            HMENU hMenu = CreatePopupMenu();
            
            // Colors Submenu
            HMENU hColorMenu = CreatePopupMenu();
            
            auto addColorMenu = [&](HMENU hParent, const char* name, int baseId) {
                HMENU hSub = CreatePopupMenu();
                AppendMenuA(hSub, MF_STRING, baseId, "Red");
                AppendMenuA(hSub, MF_STRING, baseId + 1, "Green");
                AppendMenuA(hSub, MF_STRING, baseId + 2, "Blue");
                AppendMenuA(hSub, MF_STRING, baseId + 3, "White");
                AppendMenuA(hParent, MF_POPUP, (UINT_PTR)hSub, name);
            };

            addColorMenu(hColorMenu, "GPU Color", IDM_COLOR_GPU_RED);
            addColorMenu(hColorMenu, "Temp Color", IDM_COLOR_TEMP_RED);
            addColorMenu(hColorMenu, "Mem Color", IDM_COLOR_MEM_RED);

            AppendMenuA(hMenu, MF_POPUP, (UINT_PTR)hColorMenu, "Colors");
            
            // Autorun
            UINT flags = MF_STRING;
            if (ctx->settings.IsAutoRunEnabled()) flags |= MF_CHECKED;
            AppendMenuA(hMenu, flags, IDM_AUTORUN, "Run at Startup");

            AppendMenuA(hMenu, MF_SEPARATOR, 0, NULL);
            AppendMenuA(hMenu, MF_STRING, IDM_EXIT, "Exit");
            
            SetForegroundWindow(hwnd); 
            TrackPopupMenu(hMenu, TPM_BOTTOMALIGN | TPM_LEFTALIGN, pt.x, pt.y, 0, hwnd, NULL);
            DestroyMenu(hMenu);
        }
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int main() {
    // Hide the console window
    ShowWindow(GetConsoleWindow(), SW_HIDE);

    AppContext ctx;

    // Register Window Class
    const char CLASS_NAME[] = "GPUMonitorTaskbarOverlay";
    WNDCLASSA wc = { };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH); 
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    RegisterClassA(&wc);

    // Find Taskbar Position
    HWND hTaskbar = FindWindowA("Shell_TrayWnd", NULL);
    RECT rcTaskbar;
    if (hTaskbar) {
        GetWindowRect(hTaskbar, &rcTaskbar);
    } else {
        rcTaskbar = {0, 0, 1920, 40};
    }

    HWND hTray = FindWindowExA(hTaskbar, NULL, "TrayNotifyWnd", NULL);
    RECT rcTray = {0};
    bool foundTray = false;
    if (hTray) {
        GetWindowRect(hTray, &rcTray);
        foundTray = true;
    }

    int taskbarHeight = rcTaskbar.bottom - rcTaskbar.top;
    int width = 300; 
    int height = taskbarHeight;
    
    int x, y;

    if ((rcTaskbar.right - rcTaskbar.left) > taskbarHeight) {
        y = rcTaskbar.top; 
        if (foundTray && rcTray.left > rcTaskbar.left) {
            x = rcTray.left - width - 10; 
        } else {
            x = rcTaskbar.right - width - 150; 
        }
    } else {
        x = rcTaskbar.left;
        y = rcTaskbar.bottom - height - 10;
    }

    HWND hwnd = CreateWindowExA(
        WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW, 
        CLASS_NAME,
        "NVBar",
        WS_POPUP, 
        x, y, width, height,
        NULL,
        NULL,
        GetModuleHandle(NULL),
        &ctx // Pass context here
    );

    if (hwnd == NULL) {
        return 0;
    }

    UpdateLayeredWindowContent(hwnd, &ctx);
    ShowWindow(hwnd, SW_SHOW);

    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
