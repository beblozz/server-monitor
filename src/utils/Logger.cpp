#include "Logger.h"

#include <iostream>
#include <ctime>

void Logger::info(const std::string& message)
{
    std::cout
        << "[INFO] "
        << message
        << std::endl;
}

void Logger::error(const std::string& message)
{
    std::cerr
        << "[ERROR] "
        << message
        << std::endl;
}

void Logger::warning(const std::string& message)
{
    std::cout
        << "[WARNING] "
        << message
        << std::endl;
}