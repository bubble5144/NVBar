#include "GpuMonitor.hpp"
#include <stdio.h>

GpuMonitor::GpuMonitor() {
    nvmlReturn_t result = nvmlInit();
    if (result != NVML_SUCCESS) {
        initialized = false;
        return;
    }

    unsigned int deviceCount = 0;
    result = nvmlDeviceGetCount(&deviceCount);
    if (result != NVML_SUCCESS || deviceCount == 0) {
        nvmlShutdown();
        initialized = false;
        return;
    }

    result = nvmlDeviceGetHandleByIndex(0, &device);
    if (result != NVML_SUCCESS) {
        nvmlShutdown();
        initialized = false;
        return;
    }

    initialized = true;
    Update();
}

GpuMonitor::~GpuMonitor() {
    if (initialized) {
        nvmlShutdown();
    }
}

void GpuMonitor::Update() {
    if (!initialized) return;

    nvmlReturn_t result;
    
    // 获取GPU和内存使用率
    nvmlUtilization_t utilization;
    result = nvmlDeviceGetUtilizationRates(device, &utilization);
    if (result == NVML_SUCCESS) {
        gpuUtil = utilization.gpu;
        memUtil = utilization.memory;
    } else {
        // 如果获取失败，保持上次的值（不更新）
    }

    // 获取GPU温度
    result = nvmlDeviceGetTemperature(device, NVML_TEMPERATURE_GPU, &gpuTemp);
    if (result != NVML_SUCCESS) {
        // 如果获取失败，保持上次的值（不更新）
    }
}
