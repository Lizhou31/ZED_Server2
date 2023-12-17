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
    virtual void init() = 0;
    virtual void accept() = 0;
    virtual int recvfrom() = 0;
    virtual void close() = 0;
    virtual ~Socket() {}
};

// Abstraction factory interface
class SocketFactory
{
public:
    virtual Socket *createSocket() = 0;
    virtual ~SocketFactory() {}
};

class ISystemSocket
{
public:
    virtual ~ISystemSocket() = default;
    virtual int Cbind(int sockfd, const struct sockaddr *addr, socklen_t addrlen) = 0;
    virtual int Clisten(int sockfd, int backlog) = 0;
};

// Exception
class SocketException : public std::runtime_error
{
public:
    SocketException(const std::string &message) : std::runtime_error(message) {}
};

#endif /* SOCKET_H */
