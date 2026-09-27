#include <cassert>

#include "../src/models/Metrics.h"

int main()
{
    CpuMetrics cpu;

    cpu.usage = 50.0;

    assert(cpu.usage >= 0.0);
    assert(cpu.usage <= 100.0);

    MemoryMetrics memory;

    memory.total = 1000;
    memory.used = 500;

    assert(memory.used <= memory.total);

    return 0;
}