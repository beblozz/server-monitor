#include "MemoryCollector.h"

#include <fstream>
#include <string>

MemoryMetrics MemoryCollector::collect()
{
    std::ifstream file("/proc/meminfo");

    MemoryMetrics metrics;

    if (!file.is_open())
    {
        return metrics;
    }

    std::string name;
    long value;
    std::string unit;

    long totalMemory = 0;
    long availableMemory = 0;

    while (file >> name >> value >> unit)
    {
        if (name == "MemTotal:")
        {
            totalMemory = value;
        }

        if (name == "MemAvailable:")
        {
            availableMemory = value;
        }
    }

    metrics.total = totalMemory;
    metrics.free = availableMemory;
    metrics.used = totalMemory - availableMemory;

    if (totalMemory > 0)
    {
        metrics.usage =
            static_cast<double>(metrics.used) /
            static_cast<double>(metrics.total) *
            100.0;
    }

    return metrics;
}