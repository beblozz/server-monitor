#ifndef DATABASE_H
#define DATABASE_H

#include "../models/Metrics.h"

#include <string>
#include <vector>

struct pg_conn;
typedef struct pg_conn PGconn;

struct MetricHistory
{
    int id = 0;

    std::string createdAt;

    double cpu = 0.0;
    double memory = 0.0;
    double disk = 0.0;
};

class Database
{
public:
    Database(
        const std::string& host,
        int port,
        const std::string& databaseName,
        const std::string& user,
        const std::string& password
    );

    ~Database();

    bool connect();

    void disconnect();

    bool isConnected() const;

    void saveMetrics(
        const CpuMetrics& cpu,
        const MemoryMetrics& memory,
        const DiskMetrics& disk
    );

    std::vector<MetricHistory> getHistory(
        int limit = 20
    );

private:
    PGconn* connection = nullptr;

    std::string host;
    int port;

    std::string databaseName;
    std::string user;
    std::string password;
};

#endif