#include "CpuCollector.h"

#include <fstream>
#include <sstream>
#include <thread>
#include <chrono>

CpuMetrics CpuCollector::collect()
{
    std::ifstream file("/proc/stat");

    if (!file.is_open())
    {
        return {};
    }

    std::string line;

    std::getline(file, line);

    std::istringstream stream(line);

    std::string cpu;

    unsigned long long user = 0;
    unsigned long long nice = 0;
    unsigned long long system = 0;
    unsigned long long idle = 0;
    unsigned long long iowait = 0;
    unsigned long long irq = 0;
    unsigned long long softirq = 0;
    unsigned long long steal = 0;

    stream
        >> cpu
        >> user
        >> nice
        >> system
        >> idle
        >> iowait
        >> irq
        >> softirq
        >> steal;

    unsigned long long idleTime =
        idle + iowait;

    unsigned long long totalTime =
        user +
        nice +
        system +
        idle +
        iowait +
        irq +
        softirq +
        steal;

    if (previousTotal == 0)
    {
        previousIdle = idleTime;
        previousTotal = totalTime;

        std::this_thread::sleep_for(
            std::chrono::milliseconds(100)
        );

        return collect();
    }

    unsigned long long totalDifference =
        totalTime - previousTotal;

    unsigned long long idleDifference =
        idleTime - previousIdle;

    previousTotal = totalTime;
    previousIdle = idleTime;

    double usage = 0.0;

    if (totalDifference > 0)
    {
        usage =
            100.0 *
            (1.0 -
             static_cast<double>(idleDifference) /
             static_cast<double>(totalDifference));
    }

    CpuMetrics metrics;

    metrics.usage = usage;

    return metrics;
}