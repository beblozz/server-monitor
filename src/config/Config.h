#ifndef CONFIG_H
#define CONFIG_H

#include <string>

class Config
{
public:
    bool load(const std::string& filename);

    int serverPort = 8080;

    int monitorInterval = 5;

    double cpuLimit = 80.0;
    double memoryLimit = 90.0;
    double diskLimit = 90.0;

    std::string databaseHost = "localhost";
    int databasePort = 5432;
    std::string databaseName = "server_monitor";
    std::string databaseUser = "monitor";
    std::string databasePassword = "monitor123";
};

#endif