cmake_minimum_required(VERSION 3.16)

project(ServerMonitor VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Boost REQUIRED COMPONENTS system)
find_package(PostgreSQL REQUIRED)

include_directories(src)

add_executable(server-monitor

    src/main.cpp

    src/collector/CpuCollector.cpp
    src/collector/MemoryCollector.cpp
    src/collector/DiskCollector.cpp
    src/collector/NetworkCollector.cpp
    src/collector/ProcessCollector.cpp

    src/config/Config.cpp

    src/alert/AlertManager.cpp

    src/database/Database.cpp

    src/monitor/Monitor.cpp

    src/utils/Logger.cpp

    src/server/HttpServer.cpp
)

target_link_libraries(server-monitor

    PRIVATE

    Boost::system
    PostgreSQL::PostgreSQL
)