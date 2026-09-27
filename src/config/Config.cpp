#include "Config.h"

#include <fstream>
#include <sstream>
#include <string>

namespace
{

std::string readFile(
    const std::string& filename
)
{
    std::ifstream file(filename);

    if (!file.is_open())
    {
        return "";
    }

    std::stringstream buffer;

    buffer << file.rdbuf();

    return buffer.str();
}


std::string getStringValue(
    const std::string& text,
    const std::string& key,
    const std::string& defaultValue
)
{
    std::string search =
        "\"" + key + "\"";

    std::size_t position =
        text.find(search);

    if (
        position ==
        std::string::npos
    )
    {
        return defaultValue;
    }

    position =
        text.find(
            ':',
            position
        );

    if (
        position ==
        std::string::npos
    )
    {
        return defaultValue;
    }

    position++;

    while (
        position < text.size() &&
        (
            text[position] == ' ' ||
            text[position] == '\t' ||
            text[position] == '"'
        )
    )
    {
        position++;
    }

    std::size_t end =
        position;

    while (
        end < text.size() &&
        text[end] != '"' &&
        text[end] != ',' &&
        text[end] != '\n'
    )
    {
        end++;
    }

    if (end == position)
    {
        return defaultValue;
    }

    return text.substr(
        position,
        end - position
    );
}


double getDoubleValue(
    const std::string& text,
    const std::string& key,
    double defaultValue
)
{
    std::string value =
        getStringValue(
            text,
            key,
            ""
        );

    if (value.empty())
    {
        return defaultValue;
    }

    try
    {
        return std::stod(value);
    }
    catch (...)
    {
        return defaultValue;
    }
}


int getIntValue(
    const std::string& text,
    const std::string& key,
    int defaultValue
)
{
    std::string value =
        getStringValue(
            text,
            key,
            ""
        );

    if (value.empty())
    {
        return defaultValue;
    }

    try
    {
        return std::stoi(value);
    }
    catch (...)
    {
        return defaultValue;
    }
}

}


bool Config::load(
    const std::string& filename
)
{
    std::string text =
        readFile(filename);

    if (text.empty())
    {
        return false;
    }


    serverPort =
        getIntValue(
            text,
            "server_port",
            8080
        );


    monitorInterval =
        getIntValue(
            text,
            "monitor_interval",
            5
        );


    cpuLimit =
        getDoubleValue(
            text,
            "cpu_limit",
            80.0
        );


    memoryLimit =
        getDoubleValue(
            text,
            "memory_limit",
            90.0
        );


    diskLimit =
        getDoubleValue(
            text,
            "disk_limit",
            90.0
        );


    databaseHost =
        getStringValue(
            text,
            "database_host",
            "localhost"
        );


    databasePort =
        getIntValue(
            text,
            "database_port",
            5432
        );


    databaseName =
        getStringValue(
            text,
            "database_name",
            "server_monitor"
        );


    databaseUser =
        getStringValue(
            text,
            "database_user",
            "monitor"
        );


    databasePassword =
        getStringValue(
            text,
            "database_password",
            "monitor123"
        );


    return true;
}