#ifndef MONITOR_H
#define MONITOR_H

#include "../config/Config.h"
#include "../database/Database.h"
#include "../alert/AlertManager.h"

#include <atomic>
#include <thread>


class Monitor
{
public:
    Monitor(
        Database& database,
        const Config& config
    );

    ~Monitor();

    void start();

    void stop();

    AlertManager& getAlertManager();

private:
    void run();

    Database& database;

    AlertManager alertManager;

    int monitorInterval;

    std::atomic<bool> running;

    std::thread worker;
};

#endif