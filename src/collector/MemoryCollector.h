#ifndef MEMORY_COLLECTOR_H
#define MEMORY_COLLECTOR_H

#include "../models/Metrics.h"

class MemoryCollector
{
public:
    MemoryMetrics collect();
};

#endif