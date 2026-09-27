#include "HttpServer.h"

#include "../collector/CpuCollector.h"
#include "../collector/MemoryCollector.h"
#include "../collector/DiskCollector.h"
#include "../collector/NetworkCollector.h"
#include "../collector/ProcessCollector.h"

#include <boost/asio.hpp>

#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>

using boost::asio::ip::tcp;


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

}


void HttpServer::start(
    int port,
    Database& database,
    AlertManager& alertManager
)
{
    running = true;

    try
    {
        boost::asio::io_context ioContext;


        auto serverAcceptor =
            std::make_shared<tcp::acceptor>(
                ioContext,
                tcp::endpoint(
                    tcp::v4(),
                    port
                )
            );


        acceptor = serverAcceptor;


        CpuCollector cpuCollector;
        MemoryCollector memoryCollector;
        DiskCollector diskCollector;
        NetworkCollector networkCollector;
        ProcessCollector processCollector;


        while (running)
        {
            tcp::socket socket(ioContext);


            boost::system::error_code acceptError;


            serverAcceptor->accept(
                socket,
                acceptError
            );


            /*
                If stop() closed the acceptor,
                leave the loop.
            */

            if (!running)
            {
                break;
            }


            if (acceptError)
            {
                if (
                    acceptError ==
                    boost::asio::error::operation_aborted
                )
                {
                    break;
                }

                std::cerr
                    << "HTTP accept error: "
                    << acceptError.message()
                    << std::endl;

                continue;
            }


            boost::system::error_code readError;


            char buffer[8192];


            std::size_t bytesRead =
                socket.read_some(
                    boost::asio::buffer(buffer),
                    readError
                );


            if (
                readError &&
                readError !=
                    boost::asio::error::eof
            )
            {
                continue;
            }


            std::string request(
                buffer,
                bytesRead
            );


            std::string path = "/";


            /*
                Find HTTP method
                and requested path.
            */

            std::size_t methodEnd =
                request.find(' ');


            if (
                methodEnd !=
                std::string::npos
            )
            {
                std::size_t pathEnd =
                    request.find(
                        ' ',
                        methodEnd + 1
                    );


                if (
                    pathEnd !=
                    std::string::npos
                )
                {
                    path =
                        request.substr(
                            methodEnd + 1,
                            pathEnd -
                            methodEnd -
                            1
                        );
                }
            }


            std::string body;

            std::string status =
                "200 OK";

            std::string contentType;


            /*
                Main page
            */

            if (path == "/")
            {
                body =
                    readFile(
                        "web/index.html"
                    );

                contentType =
                    "text/html; charset=UTF-8";
            }


            /*
                CSS
            */

            else if (
                path == "/style.css"
            )
            {
                body =
                    readFile(
                        "web/style.css"
                    );

                contentType =
                    "text/css; charset=UTF-8";
            }


            /*
                JavaScript
            */

            else if (
                path == "/script.js"
            )
            {
                body =
                    readFile(
                        "web/script.js"
                    );

                contentType =
                    "application/javascript; charset=UTF-8";
            }


            /*
                Current metrics
            */

            else if (
                path == "/api/metrics"
            )
            {
                CpuMetrics cpu =
                    cpuCollector.collect();


                MemoryMetrics memory =
                    memoryCollector.collect();


                DiskMetrics disk =
                    diskCollector.collect();


                std::vector<
                    NetworkInterface
                > networks =
                    networkCollector.collect();


                std::vector<
                    ProcessInfo
                > processes =
                    processCollector.collect();


                std::stringstream json;


                json << "{";


                /*
                    CPU
                */

                json << "\"cpu\":";
                json << cpu.usage;
                json << ",";


                /*
                    Memory
                */

                json << "\"memory\":";
                json << memory.usage;
                json << ",";


                /*
                    Disk
                */

                json << "\"disk\":";
                json << disk.usage;
                json << ",";


                /*
                    Network
                */

                json << "\"network\":[";


                for (
                    std::size_t i = 0;
                    i < networks.size();
                    ++i
                )
                {
                    json << "{";


                    json << "\"name\":\"";
                    json << networks[i].name;
                    json << "\",";


                    json << "\"received\":";
                    json << networks[i].received;
                    json << ",";


                    json << "\"transmitted\":";
                    json << networks[i].transmitted;


                    json << "}";


                    if (
                        i + 1 <
                        networks.size()
                    )
                    {
                        json << ",";
                    }
                }


                json << "],";


                /*
                    Processes
                */

                json << "\"processes\":[";


                for (
                    std::size_t i = 0;
                    i < processes.size();
                    ++i
                )
                {
                    json << "{";


                    json << "\"pid\":";
                    json << processes[i].pid;
                    json << ",";


                    json << "\"name\":\"";
                    json << processes[i].name;
                    json << "\",";


                    json << "\"cpu\":";
                    json << processes[i].cpu;
                    json << ",";


                    json << "\"memory\":";
                    json << processes[i].memory;


                    json << "}";


                    if (
                        i + 1 <
                        processes.size()
                    )
                    {
                        json << ",";
                    }
                }


                json << "]";


                json << "}";


                body =
                    json.str();


                contentType =
                    "application/json; charset=UTF-8";
            }


            /*
                System status
            */

            else if (
                path == "/api/status"
            )
            {
                std::stringstream json;


                json << "{";


                json << "\"status\":\"online\",";


                json << "\"database\":\"";


                if (
                    database.isConnected()
                )
                {
                    json << "connected";
                }
                else
                {
                    json << "disconnected";
                }


                json << "\",";


                json << "\"cpu\":\"";

                json <<
                    alertManager.getCpuStatus();

                json << "\",";


                json << "\"memory\":\"";

                json <<
                    alertManager.getMemoryStatus();

                json << "\",";


                json << "\"disk\":\"";

                json <<
                    alertManager.getDiskStatus();

                json << "\"";


                json << "}";


                body =
                    json.str();


                contentType =
                    "application/json; charset=UTF-8";
            }


            /*
                Metric history
            */

            else if (
                path == "/api/history"
            )
            {
                std::vector<
                    MetricHistory
                > history =
                    database.getHistory(20);


                std::stringstream json;


                json << "[";


                for (
                    std::size_t i = 0;
                    i < history.size();
                    ++i
                )
                {
                    json << "{";


                    json << "\"id\":";
                    json << history[i].id;
                    json << ",";


                    json << "\"created_at\":\"";
                    json << history[i].createdAt;
                    json << "\",";


                    json << "\"cpu\":";
                    json << history[i].cpu;
                    json << ",";


                    json << "\"memory\":";
                    json << history[i].memory;
                    json << ",";


                    json << "\"disk\":";
                    json << history[i].disk;


                    json << "}";


                    if (
                        i + 1 <
                        history.size()
                    )
                    {
                        json << ",";
                    }
                }


                json << "]";


                body =
                    json.str();


                contentType =
                    "application/json; charset=UTF-8";
            }


            /*
                404
            */

            else
            {
                status =
                    "404 Not Found";


                body =
                    "404 - Page not found";


                contentType =
                    "text/plain; charset=UTF-8";
            }


            /*
                Build HTTP response
            */

            std::stringstream response;


            response
                << "HTTP/1.1 "
                << status
                << "\r\n";


            response
                << "Content-Type: "
                << contentType
                << "\r\n";


            response
                << "Content-Length: "
                << body.size()
                << "\r\n";


            response
                << "Connection: close"
                << "\r\n";


            response
                << "\r\n";


            response
                << body;


            boost::system::error_code writeError;


            boost::asio::write(
                socket,
                boost::asio::buffer(
                    response.str()
                ),
                writeError
            );
        }


        /*
            Clear acceptor after shutdown.
        */

        acceptor.reset();
    }
    catch (
        const std::exception& exception
    )
    {
        std::cerr
            << "HTTP server error: "
            << exception.what()
            << std::endl;


        acceptor.reset();
    }


    running = false;
}


void HttpServer::stop()
{
    running = false;


    if (acceptor)
    {
        boost::system::error_code error;


        acceptor->close(error);
    }
}