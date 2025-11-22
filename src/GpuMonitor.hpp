#pragma once
#include <nvml.h>

class GpuMonitor {
public:
    GpuMonitor();
    ~GpuMonitor();

    void Update();
    
    unsigned int GetGpuUtil() const { return gpuUtil; }
    unsigned int GetMemUtil() const { return memUtil; }
    unsigned int GetTemp() const { return gpuTemp; }
    bool IsInitialized() const { return initialized; }

private:
    nvmlDevice_t device;
    bool initialized = false;
    unsigned int gpuUtil = 0;
    unsigned int memUtil = 0;
    unsigned int gpuTemp = 0;
};
