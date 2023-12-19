#ifndef SOCKETSERVER_H
#define SOCKETSERVER_H

#include "Socket.hpp"
class SocketServerTest;

class SocketServer
{
public:
    friend class ::SocketServerTest;
    SocketServer(SocketFactory *factory) : factory(factory), socket(nullptr){};
    void init(int port);
    void waiting_connection();
    int getCommand();
    void shutdown();
    ~SocketServer()
    {
        shutdown();
    };

private:
    SocketFactory *factory;
    Socket *socket;
    std::string message;
};

#endif // SOCKETSERVER_H