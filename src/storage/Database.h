#ifndef DATABASE_H
#define DATABASE_H

#include "../models/Metrics.h"

struct pg_conn;
typedef struct pg_conn PGconn;

class Database
{
public:
    ~Database();

    bool connect();
    void disconnect();

    bool isConnected() const;

    void saveMetrics(
        const CpuMetrics& cpu,
        const MemoryMetrics& memory,
        const DiskMetrics& disk
    );

private:
    PGconn* connection = nullptr;
};

#endif