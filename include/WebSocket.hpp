/**
 * @file WebSocket.hpp
 * @author Lizhou (lisie31s@gmail.com)
 * @brief  Defines the WebSocket class
 * @date 2024-01-13
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef WEBSOCKET_H
#define WEBSOCKET_H

#include "Socket.hpp"
#include <netinet/in.h>
#include <netdb.h>
#include <cstring>
class WebSocketTest;

namespace myWebSocket
{
    /**
     * @class WebSocket
     *
     * @brief This inherited the class Socket virtual and implement it with C API.
     */
    class WebSocket : public Socket
    {
    public:
        /**
         * @brief Friend class to google test.
         */
        friend class ::WebSocketTest;

        /**
         * @brief Construct a new WebSocket object, refer to Linux socket programming.
         *
         * @param sys   The API wrapper for C socket functions
         * @param port  The Socket port
         */
        WebSocket(ISystemSocket *sys, int port) : systemSocket(sys)
        {
            bzero(&_server_address, sizeof(_server_address));
            _server_address.sin_family = AF_INET;
            _server_address.sin_addr.s_addr = htonl(INADDR_ANY);
            _server_address.sin_port = htons(port);
            _server_fd = socket(AF_INET, SOCK_STREAM, 0);
        }

        /**
         * @brief Initialize the websocket server
         */
        void m_init() override;

        /**
         * @brief Websocet accept (Blocking)
         */
        void m_accept() override;

        /**
         * @brief Receive data from the established coonection (Blocking)
         *
         * @param message std::string to store the data received
         */
        void m_recvfrom(::std::string &message) override;

        /**
         * @brief Send data to the established connection (Blocking)
         *
         * @param message std::string to store the data being sent
         */
        void m_sendto(::std::string &message) override;

        /**
         * @brief Close the socket connect (with client)
         */
        void m_closeConnection() override;

        /**
         * @brief CLose the socket server
         */
        void m_closeServer() override;

    private:
        /**
         * @brief Pinter to the wapper of C socket funcions
         */
        ISystemSocket *systemSocket;

        /**
         * @brief File descript of server and client.
         */
        int _server_fd, _client_fd;

        /**
         * @brief Socket address of server and client
         */
        struct sockaddr_in _server_address, _client_address;

        /**
         * @brief Client address len for recvfrom
         */
        socklen_t _client_address_len;

        /**
         * @brief A buffer for socket recvfrom
         */
        char _buf[2048];
    };

    /**
     * @class SystemSocket
     *
     * @brief A wrapper for Linux system socket in C, all return and parameter refer to man page.
     */
    class SystemSocket : public ISystemSocket
    {
    public:
        /**
         * @brief Wrapper of libc bind, bind a name to a socket
         *
         * @param sockfd
         * @param addr
         * @param addrlen
         * @return int
         */
        int Cbind(int sockfd, const struct sockaddr *addr, socklen_t addrlen) override
        {
            return bind(sockfd, addr, addrlen);
        }

        /**
         * @brief Wrapper of libc listen, listen for connections on a socket
         * 
         * @param sockfd 
         * @param backlog 
         * @return int 
         */
        int Clisten(int sockfd, int backlog) override
        {
            return listen(sockfd, backlog);
        }

        /**
         * @brief Wrapper of libc accept, accept a connection on a socket
         * 
         * @param sockfd 
         * @param addr 
         * @param addrlen 
         * @return int 
         */
        int Caccept(int sockfd, struct sockaddr *addr, socklen_t *addrlen) override
        {
            return accept(sockfd, addr, addrlen);
        }

        /**
         * @brief Wrapper of libc recvfrom, receive a message from a socket
         * 
         * @param socket 
         * @param buffer 
         * @param length 
         * @param flags 
         * @param address 
         * @param address_len 
         * @return ssize_t 
         */
        ssize_t Crecvfrom(int socket, char *buffer, size_t length, int flags,
                          struct sockaddr *address, socklen_t *address_len) override
        {
            return recvfrom(socket, (void *)buffer, length, flags, address, address_len);
        }

        /**
         * @brief Wrapper of libc sendto, send a message on a socket
         * 
         * @param socket 
         * @param message 
         * @param length 
         * @param flags 
         * @param dest_addr 
         * @param dest_len 
         * @return ssize_t 
         */
        ssize_t Csendto(int socket, const void *message, size_t length,
                        int flags, const struct sockaddr *dest_addr,
                        socklen_t dest_len) override
        {
            return sendto(socket, message, length, flags, dest_addr, dest_len);
        }

        /**
         * @brief Wrapper of libc close, close a file descriptor
         * 
         * @param sockfd 
         * @return int 
         */
        int Cclose(int sockfd) override
        {
            return close(sockfd);
        }
    };

    /**
     * @class WebSocketFactory
     * 
     * @brief Factory pattern for the WebSocket implementation
     */
    class WebSocketFactory : public SocketFactory
    {
    public:

        /**
         * @brief Create a Socket object
         * 
         * @param port 
         * @return Socket* 
         */
        Socket *createSocket(int port) override
        {
            return new WebSocket(&sys, port);
        }

    private:

        /**
         * @brief Wrapper of system socket
         * 
         */
        SystemSocket sys;
    };

}

#endif /* WEBSOCKET_H */