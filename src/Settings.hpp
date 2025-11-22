#pragma once
#include <windows.h>

/**
 * 设置管理类
 * 处理开机自启动和颜色设置的持久化
 */
class Settings {
public:
    Settings();

    /** 检查是否启用了开机自启动 */
    bool IsAutoRunEnabled();
    
    /** 设置开机自启动 */
    void SetAutoRun(bool enable);

    // 颜色设置（公开成员变量，便于直接访问）
    COLORREF colorGpu;   // GPU使用率显示颜色
    COLORREF colorTemp;  // 温度显示颜色
    COLORREF colorMem;   // 内存使用率显示颜色

    /** 从注册表加载保存的颜色设置 */
    void LoadColors();
    
    /** 将当前颜色设置保存到注册表 */
    void SaveColors();
};
