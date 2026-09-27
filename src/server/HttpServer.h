#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include "../database/Database.h"
#include "../alert/AlertManager.h"

#include <boost/asio.hpp>

#include <atomic>
#include <memory>

class HttpServer
{
public:
    void start(
        int port,
        Database& database,
        AlertManager& alertManager
    );

    void stop();

private:
    std::atomic<bool> running{false};

    std::shared_ptr<
        boost::asio::ip::tcp::acceptor
    > acceptor;
};

#endif