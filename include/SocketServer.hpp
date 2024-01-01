#ifndef SOCKETSERVER_H
#define SOCKETSERVER_H

#include "Socket.hpp"
#include "CommandSolver.hpp"
class SocketServerTest;

class SocketServer
{
public:
    friend class ::SocketServerTest;
    SocketServer(std::unique_ptr<::SocketFactory> _factory,
                 std::shared_ptr<simplepubsub::IPublisher> invoker_pub) : factory(std::move(_factory)),
                                                                          socket(nullptr),
                                                                          invoker(invoker_pub){}
    void init(int port);
    void waiting_connection(); // blocking
    void waiting_command();    // blocking
    void shutdown();
    ~SocketServer()
    {
        shutdown();
    };

private:
    std::unique_ptr<::SocketFactory> factory;
    Socket *socket;
    std::string message;
    commandsolver::CommandInvoker invoker;
};

#endif // SOCKETSERVER_H