#ifndef ALERT_MANAGER_H
#define ALERT_MANAGER_H

#include "../models/Metrics.h"

#include <string>

class AlertManager
{
public:
    AlertManager(
        double cpuLimit,
        double memoryLimit,
        double diskLimit
    );

    void check(
        const CpuMetrics& cpu,
        const MemoryMetrics& memory,
        const DiskMetrics& disk
    );

    bool isCpuAlert() const;
    bool isMemoryAlert() const;
    bool isDiskAlert() const;

    std::string getCpuStatus() const;
    std::string getMemoryStatus() const;
    std::string getDiskStatus() const;

private:
    double cpuLimit;
    double memoryLimit;
    double diskLimit;

    bool cpuAlert = false;
    bool memoryAlert = false;
    bool diskAlert = false;
};

#endif