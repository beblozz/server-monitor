#include "HttpServer.h"

#include "../utils/Logger.h"

void HttpServer::start(int port)
{
    Logger::info(
        "HTTP server started on port " +
        std::to_string(port)
    );
}