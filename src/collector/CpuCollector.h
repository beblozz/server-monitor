#ifndef CPU_COLLECTOR_H
#define CPU_COLLECTOR_H

#include "../models/Metrics.h"

class CpuCollector
{
public:
    CpuMetrics collect();

private:
    unsigned long long previousIdle = 0;
    unsigned long long previousTotal = 0;
};

#endif