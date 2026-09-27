#ifndef DISK_COLLECTOR_H
#define DISK_COLLECTOR_H

#include "../models/Metrics.h"

#include <string>

class DiskCollector
{
public:
    DiskMetrics collect(const std::string& path = "/");
};

#endif