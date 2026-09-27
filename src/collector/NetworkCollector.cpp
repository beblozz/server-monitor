#include "NetworkCollector.h"

#include <fstream>
#include <sstream>

std::vector<NetworkInterface> NetworkCollector::collect()
{
    std::vector<NetworkInterface> interfaces;

    std::ifstream file("/proc/net/dev");

    if (!file.is_open())
    {
        return interfaces;
    }

    std::string line;

    while (std::getline(file, line))
    {
        if (line.find(":") == std::string::npos)
        {
            continue;
        }

        std::istringstream stream(line);

        std::string interfaceName;

        stream >> interfaceName;

        if (!interfaceName.empty() &&
            interfaceName.back() == ':')
        {
            interfaceName.pop_back();
        }

        unsigned long long received = 0;
        unsigned long long packets = 0;
        unsigned long long errors = 0;
        unsigned long long dropped = 0;
        unsigned long long fifo = 0;
        unsigned long long frame = 0;
        unsigned long long compressed = 0;
        unsigned long long multicast = 0;

        unsigned long long transmitted = 0;

        stream
            >> received
            >> packets
            >> errors
            >> dropped
            >> fifo
            >> frame
            >> compressed
            >> multicast
            >> transmitted;

        NetworkInterface network;

        network.name = interfaceName;
        network.received = received;
        network.transmitted = transmitted;

        interfaces.push_back(network);
    }

    return interfaces;
}