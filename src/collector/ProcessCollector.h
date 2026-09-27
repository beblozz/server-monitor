#ifndef PROCESS_COLLECTOR_H
#define PROCESS_COLLECTOR_H

#include "../models/Metrics.h"

#include <vector>

class ProcessCollector
{
public:
    std::vector<ProcessInfo> collect();
};

#endif