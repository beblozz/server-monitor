#include "AlertManager.h"

#include "../utils/Logger.h"

AlertManager::AlertManager(
    double cpuLimit,
    double memoryLimit,
    double diskLimit
)
    : cpuLimit(cpuLimit),
      memoryLimit(memoryLimit),
      diskLimit(diskLimit)
{
}

void AlertManager::check(
    const CpuMetrics& cpu,
    const MemoryMetrics& memory,
    const DiskMetrics& disk
)
{
    /*
        CPU
    */

    if (cpu.usage >= cpuLimit)
    {
        if (!cpuAlert)
        {
            Logger::error(
                "WARNING: CPU usage is above limit"
            );

            cpuAlert = true;
        }
    }
    else
    {
        if (cpuAlert)
        {
            Logger::info(
                "CPU usage returned to normal"
            );

            cpuAlert = false;
        }
    }


    /*
        MEMORY
    */

    if (memory.usage >= memoryLimit)
    {
        if (!memoryAlert)
        {
            Logger::error(
                "WARNING: Memory usage is above limit"
            );

            memoryAlert = true;
        }
    }
    else
    {
        if (memoryAlert)
        {
            Logger::info(
                "Memory usage returned to normal"
            );

            memoryAlert = false;
        }
    }


    /*
        DISK
    */

    if (disk.usage >= diskLimit)
    {
        if (!diskAlert)
        {
            Logger::error(
                "WARNING: Disk usage is above limit"
            );

            diskAlert = true;
        }
    }
    else
    {
        if (diskAlert)
        {
            Logger::info(
                "Disk usage returned to normal"
            );

            diskAlert = false;
        }
    }
}


bool AlertManager::isCpuAlert() const
{
    return cpuAlert;
}


bool AlertManager::isMemoryAlert() const
{
    return memoryAlert;
}


bool AlertManager::isDiskAlert() const
{
    return diskAlert;
}


std::string AlertManager::getCpuStatus() const
{
    if (cpuAlert)
    {
        return "warning";
    }

    return "ok";
}


std::string AlertManager::getMemoryStatus() const
{
    if (memoryAlert)
    {
        return "warning";
    }

    return "ok";
}


std::string AlertManager::getDiskStatus() const
{
    if (diskAlert)
    {
        return "warning";
    }

    return "ok";
}