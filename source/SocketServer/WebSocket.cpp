#include "WebSocket.hpp"

namespace myWebSocket
{
    void WebSocket::init()
    {
        int ret;
        ret = systemSocket->Cbind(_server_fd, (struct sockaddr*)&_server_address, sizeof(_server_address));
        ret = systemSocket->Clisten(_server_fd, 1);
    }
    void WebSocket::accept()
    {
    }
    int WebSocket::recvfrom()
    {
    }
    void WebSocket::close()
    {
    }
}
