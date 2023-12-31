#ifndef WEBSOCKET_H
#define WEBSOCKET_H

#include "Socket.hpp"
#include <netinet/in.h>
#include <netdb.h>
#include <cstring>
class WebSocketTest;

namespace myWebSocket
{
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
            _server_address.sin_port = htons(port);
            _server_fd = socket(AF_INET, SOCK_STREAM, 0);
        }
        void m_init() override;
        void m_accept() override;
        void m_recvfrom(::std::string &message) override;
        void m_sendto(::std::string &message) override;
        void m_closeConnection() override;
        void m_closeServer() override;

    private:
        ISystemSocket *systemSocket;
        int _server_fd, _client_fd;
        struct sockaddr_in _server_address, _client_address;
        socklen_t _client_address_len;
        char _buf[2048];
    };

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
        ssize_t Crecvfrom(int socket, char *buffer, size_t length, int flags,
                          struct sockaddr *address, socklen_t *address_len) override
        {
            return recvfrom(socket, (void *)buffer, length, flags, address, address_len);
        }

        ssize_t Csendto(int socket, const void *message, size_t length,
                        int flags, const struct sockaddr *dest_addr,
                        socklen_t dest_len) override
        {
            return sendto(socket, message, length, flags, dest_addr, dest_len);
        }

        int Cclose(int sockfd) override
        {
            return close(sockfd);
        }
    };

    // Concrete factory for Web Socket
    class WebSocketFactory : public SocketFactory
    {
    public:
        Socket *createSocket(int port) override
        {
            return new WebSocket(&sys, port);
        }

    private:
        SystemSocket sys;
    };

}

#endif /* WEBSOCKET_H */