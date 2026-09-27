#include "DiskCollector.h"

#include <sys/statvfs.h>

DiskMetrics DiskCollector::collect(const std::string& path)
{
    DiskMetrics metrics;

    struct statvfs stat;

    if (statvfs(path.c_str(), &stat) != 0)
    {
        return metrics;
    }

    unsigned long long blockSize = stat.f_frsize;

    unsigned long long totalBlocks = stat.f_blocks;
    unsigned long long freeBlocks = stat.f_bfree;
    unsigned long long availableBlocks = stat.f_bavail;

    metrics.total =
        totalBlocks * blockSize;

    metrics.free =
        availableBlocks * blockSize;

    metrics.used =
        metrics.total -
        freeBlocks * blockSize;

    if (metrics.total > 0)
    {
        metrics.usage =
            static_cast<double>(metrics.used) /
            static_cast<double>(metrics.total) *
            100.0;
    }

    return metrics;
}