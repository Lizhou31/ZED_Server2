#ifndef WEBSOCKET_TEST_H
#define WEBSOCKET_TEST_H

#include "WebSocket.hpp"
#include <gtest/gtest.h>
#include <gmock/gmock.h>

class MockSystemSocket : public ISystemSocket
{
public:
    MOCK_METHOD(int, Cbind, (int, const struct sockaddr *, socklen_t), (override));
    MOCK_METHOD(int, Clisten, (int, int), (override));
};

class WebSocketTest : public ::testing::Test
{
protected:
    WebSocketTest() {}
    ~WebSocketTest() override
    {
    }
    void SetUp() override
    {
        ws = new myWebSocket::WebSocket(&mockSys, 8888);
    }

    void TearDown() override
    {
        delete ws;
    }

    void getCbindParam(int *_serverfd, struct sockaddr_in *_server_addr)
    {
        *_serverfd = ws->_server_fd;
        _server_addr = &(ws->_server_address);
    }
    myWebSocket::WebSocket *ws;
    MockSystemSocket mockSys;
};

#endif /* WEBSOCKET_TEST_H */