/**
 * @file websocket_test.h
 * @author Lizhou (lisie31s@gmail.com)
 * @brief Websocket test header
 * @version 0.1
 * @date 2024-01-15
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef WEBSOCKET_TEST_H
#define WEBSOCKET_TEST_H

#include "WebSocket.hpp"
#include <gtest/gtest.h>
#include <gmock/gmock.h>

/**
 * @brief The system socket mock
 */
class MockSystemSocket : public ISystemSocket
{
public:
    MOCK_METHOD(int, Cbind, (int, const struct sockaddr *, socklen_t), (override));
    MOCK_METHOD(int, Clisten, (int, int), (override));
    MOCK_METHOD(int, Caccept, (int, struct sockaddr *, socklen_t *), (override));
    MOCK_METHOD(ssize_t, Crecvfrom, (int, char *, size_t, int, struct sockaddr *, socklen_t *), (override));
    MOCK_METHOD(ssize_t, Csendto, (int, const void *, size_t, int, const struct sockaddr *, socklen_t), (override));
    MOCK_METHOD(int, Cclose, (int), (override));
};

/**
 * @brief WebSocket test class
 */
class WebSocketTest : public ::testing::Test
{
protected:
    /**
     * @brief Construct a new Web Socket Test object
     */
    WebSocketTest() {}

    /**
     * @brief Destroy the Web Socket Test object
     */
    ~WebSocketTest() override {}

    /**
     * @brief Set the Up object
     */
    void SetUp() override
    {
        ws = std::make_unique<myWebSocket::WebSocket>(&mockSys, 8888);
    }

    /**
     * @brief TearDown the test
     * 
     */
    void TearDown() override
    {
    }

    /**
     * @brief Get client file descriptor
     * 
     * @return int 
     */
    int getclientfd()
    {
        return ws->_client_fd;
    }

    /**
     * @brief Get the buffer p object
     * 
     * @return char* 
     */
    char *getbuffer_p()
    {
        return ws->_buf;
    }

    /**
     * @brief System socket mock
     * 
     */
    MockSystemSocket mockSys;

    /**
     * @brief Websocket unique pointer
     * 
     */
    std::unique_ptr<myWebSocket::WebSocket> ws;
};

#endif /* WEBSOCKET_TEST_H */