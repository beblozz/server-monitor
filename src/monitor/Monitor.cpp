#include "Monitor.h"

#include "../collector/CpuCollector.h"
#include "../collector/MemoryCollector.h"
#include "../collector/DiskCollector.h"
#include "../utils/Logger.h"

#include <chrono>

Monitor::Monitor(
    Database& database,
    const Config& config
)
    : database(database),
      alertManager(
          config.cpuLimit,
          config.memoryLimit,
          config.diskLimit
      ),
      running(false)
{
}

Monitor::~Monitor()
{
    stop();
}

void Monitor::start()
{
    if (running)
    {
        return;
    }

    running = true;

    worker = std::thread(
        &Monitor::run,
        this
    );

    Logger::info(
        "Background monitor started"
    );
}

void Monitor::stop()
{
    if (!running)
    {
        return;
    }

    running = false;

    if (worker.joinable())
    {
        worker.join();
    }

    Logger::info(
        "Background monitor stopped"
    );
}

void Monitor::run()
{
    CpuCollector cpuCollector;
    MemoryCollector memoryCollector;
    DiskCollector diskCollector;

    while (running)
    {
        /*
            Collect CPU
        */

        CpuMetrics cpu =
            cpuCollector.collect();


        /*
            Collect memory
        */

        MemoryMetrics memory =
            memoryCollector.collect();


        /*
            Collect disk
        */

        DiskMetrics disk =
            diskCollector.collect();


        /*
            Check system limits
        */

        alertManager.check(
            cpu,
            memory,
            disk
        );


        /*
            Save metrics
            to PostgreSQL
        */

        database.saveMetrics(
            cpu,
            memory,
            disk
        );

        Logger::info(
            "Metrics saved to PostgreSQL"
        );


        /*
            Wait 5 seconds.

            Instead of one 5-second sleep,
            we use 50 small sleeps.

            This allows the monitor
            to stop faster.
        */

        for (
            int i = 0;
            i < 50 && running;
            ++i
        )
        {
            std::this_thread::sleep_for(
                std::chrono::milliseconds(100)
            );
        }
    }
}


/*
    Give HTTP server access
    to the same AlertManager.
*/

AlertManager& Monitor::getAlertManager()
{
    return alertManager;
}