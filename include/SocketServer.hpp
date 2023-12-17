#ifndef SOCKETSERVER_H
#define SOCKETSERVER_H

#include "Socket.hpp"


class SocketServer {
private:
    SocketFactory* factory;
    Socket* socket;

public:
    SocketServer(SocketFactory* factory): factory(factory), socket(nullptr){};
    void init(int port);
    void shutdown();
    ~SocketServer(){
        shutdown();
    };
};

#endif // SOCKETSERVER_H