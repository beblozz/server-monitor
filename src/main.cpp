#include "config/Config.h"
#include "database/Database.h"
#include "monitor/Monitor.h"
#include "server/HttpServer.h"
#include "utils/Logger.h"

#include <csignal>
#include <cstdlib>
#include <string>
#include <atomic>
#include <thread>
#include <chrono>

std::atomic<bool> shutdownRequested(false);

void handleSignal(int signal)
{
    if (signal == SIGINT)
    {
        shutdownRequested = true;
    }
}

int main()
{
    Logger::info(
        "Application started"
    );

    std::signal(
        SIGINT,
        handleSignal
    );

    Config config;

    const char* configEnvironment =
        std::getenv(
            "SERVER_MONITOR_CONFIG"
        );

    std::string configFile =
        "config/config.json";

    if (configEnvironment != nullptr)
    {
        configFile =
            configEnvironment;
    }

    if (
        !config.load(
            configFile
        )
    )
    {
        Logger::error(
            "Failed to load configuration"
        );

        return 1;
    }

    Logger::info(
        "Configuration loaded"
    );

    Database database(
        config.databaseHost,
        config.databasePort,
        config.databaseName,
        config.databaseUser,
        config.databasePassword
    );

    if (
        !database.connect()
    )
    {
        Logger::error(
            "Failed to connect to PostgreSQL"
        );

        return 1;
    }

    Monitor monitor(
        database,
        config
    );

    monitor.start();

    Logger::info(
        "Collecting system metrics..."
    );

    HttpServer server;

    std::thread serverThread(
        [&]()
        {
            server.start(
                config.serverPort,
                database,
                monitor.getAlertManager()
            );
        }
    );

    while (!shutdownRequested)
    {
        std::this_thread::sleep_for(
            std::chrono::milliseconds(100)
        );
    }

    Logger::info(
        "Shutdown signal received"
    );

    server.stop();

    if (
        serverThread.joinable()
    )
    {
        serverThread.join();
    }

    monitor.stop();

    Logger::info(
        "Application stopped"
    );

    return 0;
}