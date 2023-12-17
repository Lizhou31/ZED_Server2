#include "WebSocket.hpp"

namespace myWebSocket
{
    void WebSocket::init()
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
    void WebSocket::accept()
    {
        _client_fd = systemSocket->Caccept(_server_fd, (struct sockaddr *)&_client_address, &_client_address_len);
        if (_client_fd == -1)
        {
            throw SocketException("Socket accept error");
        }
    }
    int WebSocket::recvfrom()
    {
    }
    void WebSocket::close()
    {
    }
}
