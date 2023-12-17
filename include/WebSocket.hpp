#ifndef WEBSOCKET_H
#define WEBSOCKET_H

#include "Socket.hpp"
#include <netinet/in.h>
#include <netdb.h>
#include <cstring>
class WebSocketTest;

namespace myWebSocket
{
    class SystemSocket : public ISystemSocket
    {
    public:
        int Cbind(int sockfd, const struct sockaddr *addr, socklen_t addrlen) override
        {
            return bind(sockfd, addr, addrlen);
        }
        int Clisten(int sockfd, int backlog) override
        {
            return listen(sockfd, backlog);
        }
        int Caccept(int sockfd, struct sockaddr *addr, socklen_t *addrlen) override
        {
            return accept(sockfd, addr, addrlen);
        }
    };

    // Concrete Web Socket
    class WebSocket : public Socket
    {
    public:
        friend class ::WebSocketTest;
        WebSocket(ISystemSocket *sys, int port) : systemSocket(sys)
        {
            bzero(&_server_address, sizeof(_server_address));
            _server_address.sin_family = AF_INET;
            _server_address.sin_addr.s_addr = htonl(INADDR_ANY);
            _server_address.sin_port = port;

            _server_fd = socket(AF_INET, SOCK_STREAM, 0);
        }
        void init() override;
        void accept() override;
        int recvfrom() override;
        void close() override;

    private:
        ISystemSocket *systemSocket;
        int _server_fd, _client_fd;
        struct sockaddr_in _server_address, _client_address;
        socklen_t _client_address_len;
    };

    // Concrete factory for Web Socket
    class WebSocketFactory : public SocketFactory
    {
    public:
        SystemSocket sys;
        int port;
        Socket *createSocket() override
        {
            return new WebSocket(&sys, port);
        }
    };

}

#endif /* WEBSOCKET_H */