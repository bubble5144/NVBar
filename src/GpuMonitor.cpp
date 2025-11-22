#include "GpuMonitor.hpp"
#include <stdio.h>

GpuMonitor::GpuMonitor() {
    nvmlReturn_t result = nvmlInit();
    if (result == NVML_SUCCESS) {
        unsigned int deviceCount = 0;
        nvmlDeviceGetCount(&deviceCount);
        if (deviceCount > 0) {
            nvmlDeviceGetHandleByIndex(0, &device);
            initialized = true;
            Update();
        }
    }
}

GpuMonitor::~GpuMonitor() {
    if (initialized) {
        nvmlShutdown();
    }
}

void GpuMonitor::Update() {
    if (!initialized) return;

    nvmlReturn_t result;
    
    nvmlUtilization_t utilization;
    result = nvmlDeviceGetUtilizationRates(device, &utilization);
    if (result == NVML_SUCCESS) {
        gpuUtil = utilization.gpu;
        memUtil = utilization.memory;
    }

    result = nvmlDeviceGetTemperature(device, NVML_TEMPERATURE_GPU, &gpuTemp);
}
