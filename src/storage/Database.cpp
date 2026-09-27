#include "Database.h"

#include "../utils/Logger.h"

#include <libpq-fe.h>

#include <string>

Database::~Database()
{
    disconnect();
}

bool Database::connect()
{
    disconnect();

    connection = PQsetdbLogin(
        "localhost",
        "5432",
        nullptr,
        nullptr,
        "server_monitor",
        "monitor",
        "monitor123"
    );

    if (connection == nullptr)
    {
        Logger::error(
            "PostgreSQL: connection object is null"
        );

        return false;
    }

    if (PQstatus(connection) != CONNECTION_OK)
    {
        Logger::error(
            std::string(
                "PostgreSQL connection failed: "
            ) + PQerrorMessage(connection)
        );

        disconnect();

        return false;
    }

    Logger::info(
        "PostgreSQL connected successfully"
    );

    return true;
}

void Database::disconnect()
{
    if (connection != nullptr)
    {
        PQfinish(connection);
        connection = nullptr;
    }
}

bool Database::isConnected() const
{
    return connection != nullptr &&
           PQstatus(connection) == CONNECTION_OK;
}

void Database::saveMetrics(
    const CpuMetrics& cpu,
    const MemoryMetrics& memory,
    const DiskMetrics& disk
)
{
    if (!isConnected())
    {
        Logger::error(
            "Cannot save metrics: database is not connected"
        );

        return;
    }

    std::string cpuValue =
        std::to_string(cpu.usage);

    std::string memoryValue =
        std::to_string(memory.usage);

    std::string diskValue =
        std::to_string(disk.usage);

    const char* values[3];

    values[0] = cpuValue.c_str();
    values[1] = memoryValue.c_str();
    values[2] = diskValue.c_str();

    PGresult* result = PQexecParams(
        connection,

        "INSERT INTO metrics "
        "(cpu, memory, disk) "
        "VALUES ($1, $2, $3)",

        3,
        nullptr,
        values,
        nullptr,
        nullptr,
        0
    );

    if (result == nullptr)
    {
        Logger::error(
            "PostgreSQL returned null result"
        );

        return;
    }

    if (PQresultStatus(result) != PGRES_COMMAND_OK)
    {
        Logger::error(
            std::string(
                "Failed to save metrics: "
            ) + PQerrorMessage(connection)
        );
    }

    PQclear(result);
}