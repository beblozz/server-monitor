#ifndef METRICS_H
#define METRICS_H

#include <string>
#include <vector>

struct CpuMetrics
{
    double usage = 0.0;
};

struct MemoryMetrics
{
    long total = 0;
    long used = 0;
    long free = 0;
    double usage = 0.0;
};

struct DiskMetrics
{
    unsigned long long total = 0;
    unsigned long long used = 0;
    unsigned long long free = 0;
    double usage = 0.0;
};

struct NetworkInterface
{
    std::string name;
    unsigned long long received = 0;
    unsigned long long transmitted = 0;
};

struct ProcessInfo
{
    int pid = 0;
    std::string name;
    double cpu = 0.0;
    long memory = 0;
};

#endif