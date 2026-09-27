#ifndef NETWORK_COLLECTOR_H
#define NETWORK_COLLECTOR_H

#include "../models/Metrics.h"

#include <vector>

class NetworkCollector
{
public:
    std::vector<NetworkInterface> collect();
};

#endif