#include "WebSocket.hpp"

namespace myWebSocket
{
    void WebSocket::m_init()
    {
        int ret;
        ret = systemSocket->Cbind(_server_fd, (struct sockaddr *)&_server_address, sizeof(_server_address));
        if (ret != 0)
        {
            throw SocketException("Socket bind error");
        }

        ret = systemSocket->Clisten(_server_fd, 1);
        if (ret != 0)
        {
            throw SocketException("Socket listen error");
        }
    }
    void WebSocket::m_accept()
    {
        _client_fd = systemSocket->Caccept(_server_fd, (struct sockaddr *)&_client_address, &_client_address_len);
        if (_client_fd == -1)
        {
            throw SocketException("Socket accept error");
        }
    }
    void WebSocket::m_recvfrom(::std::string &message)
    {
        ssize_t msglen = systemSocket->Crecvfrom(_client_fd, _buf, 2048, 0,
                                                 (struct sockaddr *)&_client_address, &_client_address_len);
        if (msglen == -1)
        {
            throw SocketException("Socket recvfrom error");
        }
        // Put tailing characters
        _buf[msglen] = 0;
        message += _buf;
    }
    void WebSocket::m_closeConnection()
    {
        int ret = systemSocket->Cclose(_client_fd);
        if (ret != 0){
            throw SocketException("Socket connection close error");
        }
    }
    void WebSocket::m_closeServer()
    {
        int ret = systemSocket->Cclose(_server_fd);
        if (ret != 0){
            throw SocketException("Socket server close error");
        }
    }
}
