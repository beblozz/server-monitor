#include "Database.h"

#include "../utils/Logger.h"

#include <libpq-fe.h>

#include <iostream>
#include <string>
#include <vector>


Database::Database(
    const std::string& host,
    int port,
    const std::string& databaseName,
    const std::string& user,
    const std::string& password
)
    : host(host),
      port(port),
      databaseName(databaseName),
      user(user),
      password(password)
{
}


Database::~Database()
{
    disconnect();
}


bool Database::connect()
{
    if (connection != nullptr)
    {
        return true;
    }


    std::string portString =
        std::to_string(port);


    connection =
        PQsetdbLogin(
            host.c_str(),
            portString.c_str(),
            nullptr,
            nullptr,
            databaseName.c_str(),
            user.c_str(),
            password.c_str()
        );


    if (connection == nullptr)
    {
        Logger::error(
            "Failed to create PostgreSQL connection"
        );

        return false;
    }


    if (
        PQstatus(connection)
        != CONNECTION_OK
    )
    {
        Logger::error(
            "PostgreSQL connection failed: " +
            std::string(
                PQerrorMessage(connection)
            )
        );

        disconnect();

        return false;
    }


    Logger::info(
        "Connected to PostgreSQL"
    );


    return true;
}


void Database::disconnect()
{
    if (connection != nullptr)
    {
        PQfinish(connection);

        connection = nullptr;

        Logger::info(
            "Disconnected from PostgreSQL"
        );
    }
}


bool Database::isConnected() const
{
    return (
        connection != nullptr &&
        PQstatus(connection) == CONNECTION_OK
    );
}


void Database::saveMetrics(
    const CpuMetrics& cpu,
    const MemoryMetrics& memory,
    const DiskMetrics& disk
)
{
    if (!isConnected())
    {
        return;
    }


    std::string cpuValue =
        std::to_string(cpu.usage);

    std::string memoryValue =
        std::to_string(memory.usage);

    std::string diskValue =
        std::to_string(disk.usage);


    const char* values[3];

    values[0] =
        cpuValue.c_str();

    values[1] =
        memoryValue.c_str();

    values[2] =
        diskValue.c_str();


    PGresult* result =
        PQexecParams(
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


    if (
        result == nullptr
    )
    {
        Logger::error(
            "Failed to save metrics"
        );

        return;
    }


    if (
        PQresultStatus(result)
        != PGRES_COMMAND_OK
    )
    {
        Logger::error(
            "Failed to insert metrics: " +
            std::string(
                PQresultErrorMessage(result)
            )
        );
    }


    PQclear(result);
}


std::vector<MetricHistory>
Database::getHistory(int limit)
{
    std::vector<MetricHistory> history;


    if (!isConnected())
    {
        return history;
    }


    if (limit < 1)
    {
        limit = 1;
    }


    if (limit > 100)
    {
        limit = 100;
    }


    std::string limitValue =
        std::to_string(limit);


    const char* values[1];

    values[0] =
        limitValue.c_str();


    PGresult* result =
        PQexecParams(
            connection,

            "SELECT "
            "id, "
            "created_at, "
            "cpu, "
            "memory, "
            "disk "
            "FROM metrics "
            "ORDER BY id DESC "
            "LIMIT $1",

            1,

            nullptr,

            values,

            nullptr,

            nullptr,

            0
        );


    if (
        result == nullptr
    )
    {
        Logger::error(
            "Failed to get metric history"
        );

        return history;
    }


    if (
        PQresultStatus(result)
        != PGRES_TUPLES_OK
    )
    {
        Logger::error(
            "Failed to read metric history: " +
            std::string(
                PQresultErrorMessage(result)
            )
        );

        PQclear(result);

        return history;
    }


    int rows =
        PQntuples(result);


    for (
        int row = 0;
        row < rows;
        ++row
    )
    {
        MetricHistory item;


        item.id =
            std::stoi(
                PQgetvalue(
                    result,
                    row,
                    0
                )
            );


        item.createdAt =
            PQgetvalue(
                result,
                row,
                1
            );


        item.cpu =
            std::stod(
                PQgetvalue(
                    result,
                    row,
                    2
                )
            );


        item.memory =
            std::stod(
                PQgetvalue(
                    result,
                    row,
                    3
                )
            );


        item.disk =
            std::stod(
                PQgetvalue(
                    result,
                    row,
                    4
                )
            );


        history.push_back(item);
    }


    PQclear(result);


    return history;
}