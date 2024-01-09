#ifndef SOCKET_H
#define SOCKET_H

#include <string>
#include <stdexcept>
#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

// Socket interface
class Socket
{
public:
    virtual void m_init() = 0;
    virtual void m_accept() = 0;
    virtual void m_recvfrom(::std::string &) = 0;
    virtual void m_sendto(::std::string &) = 0;
    virtual void m_closeConnection() = 0;
    virtual void m_closeServer() = 0;
    virtual ~Socket() {}
};

class ISystemSocket
{
public:
    virtual ~ISystemSocket() = default;
    virtual int Cbind(int sockfd, const struct sockaddr *addr, socklen_t addrlen) = 0;
    virtual int Clisten(int sockfd, int backlog) = 0;
    virtual int Caccept(int sockfd, struct sockaddr *addr, socklen_t *addrlen) = 0;
    virtual ssize_t Crecvfrom(int socket, char *buffer, size_t length, int flags,
                              struct sockaddr *address, socklen_t *address_len) = 0;
    virtual ssize_t Csendto(int socket, const void *message, size_t length,
           int flags, const struct sockaddr *dest_addr,
           socklen_t dest_len) = 0;
    virtual int Cclose(int sockfd) = 0;
};

// Abstraction factory interface
class SocketFactory
{
public:
    virtual Socket *createSocket(int) = 0;
    virtual ~SocketFactory() {}
};

// Exception
class SocketException : public std::runtime_error
{
public:
    SocketException(const std::string &message) : std::runtime_error(message) {}
};

#endif /* SOCKET_H */
