/**
 * @file WebSocket.cpp
 * @author Lizhou (lisie31s@gmail.com)
 * @brief Defines the WebSocket implemetation
 * @version 0.1
 * @date 2024-01-13
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "WebSocket.hpp"

namespace myWebSocket
{
    /**
     * @brief Websocket initilization
     * 
     * @details This function contains the bind and listen, throw the exception while error occured.
     */
    void WebSocket::m_init()
    {
        int ret;
        ret = systemSocket->Cbind(_server_fd, (struct sockaddr *)&_server_address, sizeof(_server_address));
        if (ret != 0)
        {
            throw SocketException("[WebSocket Error: m_init()]: Socket bind error.");
        }

        ret = systemSocket->Clisten(_server_fd, 1);
        if (ret != 0)
        {
            throw SocketException("[WebSocket Error: m_init()]: Socket listen error.");
        }
    }

    /**
     * @brief Websocket accept
     * 
     * @details This function contains the accept, throw the exception while error occured.
     */
    void WebSocket::m_accept()
    {
        _client_fd = systemSocket->Caccept(_server_fd, (struct sockaddr *)&_client_address, &_client_address_len);
        if (_client_fd == -1)
        {
            throw SocketException("[WebSocket Error: m_accept()]: Socket accept error.");
        }
    }

    /**
     * @brief Websocket recvfrom
     * 
     * @param message store the message from client
     * 
     * @details This function contains the recvfrom, throw the exception while error occured.
     *          If no messages are available to be received and the peer has performed an orderly shutdown, 
     *          recvfrom() shall return 0. Otherwise, the function shall return -1 and set errno
     *          to indicate the error.
     * 
     * @todo Check the error code.
     */
    void WebSocket::m_recvfrom(::std::string &message)
    {
        ssize_t msglen = systemSocket->Crecvfrom(_client_fd, _buf, 2048, 0,
                                                 (struct sockaddr *)&_client_address, &_client_address_len);
        if (msglen <= 0)
        {
            throw SocketException("[WebSocket Error: m_recvfrom()]: Socket recvfrom error.");
            // TODO: Check the error code.
        }
        // Put tailing characters
        _buf[msglen] = 0;
        message += _buf;
    }

    /**
     * @brief WebSocket sendto
     * 
     * @param message the message stored to send to client
     * 
     * @details This function contains the sendto process, throw the exception while error occured.
     * 
     * @todo Check the error code.
     */
    void WebSocket::m_sendto(::std::string &message)
    {
        ssize_t msglen = systemSocket->Csendto(_client_fd, message.c_str(), message.length(), 0,
                                               (struct sockaddr *)&_client_address, sizeof(_client_address));
        if(msglen < 0){
            
            throw SocketException("[WebSocket Error: m_sendto()]: Socket sendto error");
            // TODO: Check the error code.
        }
    }

    /**
     * @brief Websocket close client connection
     * 
     * @details This function will close the client connection.
     */
    void WebSocket::m_closeConnection()
    {
        int ret = systemSocket->Cclose(_client_fd);
        if (ret != 0)
        {
            throw SocketException("[WebSocket Error: m_closeConnection()]: Socket connection close error");
        }
    }

    /**
     * @brief Websocket close the server
     * 
     * @details This function will close the serve.
     */
    void WebSocket::m_closeServer()
    {
        int ret = systemSocket->Cclose(_server_fd);
        if (ret != 0)
        {
            throw SocketException("[WebSocket Error: m_closeServer()]: Socket server close error");
        }
    }
}
