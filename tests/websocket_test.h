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
    MOCK_METHOD(int, Caccept, (int, struct sockaddr *, socklen_t *), (override));
};

class WebSocketTest : public ::testing::Test
{
protected:
    MockSystemSocket mockSys;
    std::unique_ptr<myWebSocket::WebSocket> ws;
    WebSocketTest() {}
    ~WebSocketTest() override
    {
    }
    void SetUp() override
    {
        ws = std::make_unique<myWebSocket::WebSocket>(&mockSys, 8888);
    }

    void TearDown() override
    {
    }

    int getclientfd(){
        return ws->_client_fd;
    }
};

#endif /* WEBSOCKET_TEST_H */