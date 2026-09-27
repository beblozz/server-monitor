#include "ProcessCollector.h"

#include <cctype>
#include <dirent.h>
#include <fstream>
#include <string>

std::vector<ProcessInfo> ProcessCollector::collect()
{
    std::vector<ProcessInfo> processes;

    DIR* directory = opendir("/proc");

    if (directory == nullptr)
    {
        return processes;
    }

    dirent* entry;

    while ((entry = readdir(directory)) != nullptr)
    {
        std::string directoryName = entry->d_name;

        // Нас интересуют только папки с числовым названием.
        // В Linux это PID процессов.
        if (directoryName.empty())
        {
            continue;
        }

        bool isNumber = true;

        for (char character : directoryName)
        {
            if (!std::isdigit(
                    static_cast<unsigned char>(character)
                ))
            {
                isNumber = false;
                break;
            }
        }

        if (!isNumber)
        {
            continue;
        }

        ProcessInfo process;

        process.pid =
            std::stoi(directoryName);

        std::string statusPath =
            "/proc/" +
            directoryName +
            "/status";

        std::ifstream file(statusPath);

        if (!file.is_open())
        {
            continue;
        }

        std::string line;

        while (std::getline(file, line))
        {
            if (line.rfind("Name:", 0) == 0)
            {
                process.name =
                    line.substr(5);

                // Убираем пробелы и TAB перед названием.
                while (
                    !process.name.empty() &&
                    (
                        process.name.front() == ' ' ||
                        process.name.front() == '\t'
                    )
                )
                {
                    process.name.erase(
                        process.name.begin()
                    );
                }
            }

            if (line.rfind("VmRSS:", 0) == 0)
            {
                std::string value =
                    line.substr(6);

                process.memory =
                    std::stol(value);

                break;
            }
        }

        if (!process.name.empty())
        {
            processes.push_back(process);
        }

        // Пока показываем максимум 20 процессов.
        if (processes.size() >= 20)
        {
            break;
        }
    }

    closedir(directory);

    return processes;
}