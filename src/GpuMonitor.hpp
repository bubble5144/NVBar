#pragma once
#include <nvml.h>

/**
 * GPU监控类
 * 使用NVIDIA NVML库获取GPU使用率、内存使用率和温度
 */
class GpuMonitor {
public:
    GpuMonitor();
    ~GpuMonitor();

    /**
     * 更新GPU状态信息
     * 从NVML获取最新的使用率和温度数据
     */
    void Update();
    
    /** 获取GPU使用率 (0-100%) */
    unsigned int GetGpuUtil() const { return gpuUtil; }
    
    /** 获取内存使用率 (0-100%) */
    unsigned int GetMemUtil() const { return memUtil; }
    
    /** 获取GPU温度 (摄氏度) */
    unsigned int GetTemp() const { return gpuTemp; }
    
    /** 检查NVML是否成功初始化 */
    bool IsInitialized() const { return initialized; }

private:
    nvmlDevice_t device;        // NVML设备句柄
    bool initialized = false;    // 初始化状态
    unsigned int gpuUtil = 0;    // GPU使用率
    unsigned int memUtil = 0;    // 内存使用率
    unsigned int gpuTemp = 0;    // GPU温度
};
