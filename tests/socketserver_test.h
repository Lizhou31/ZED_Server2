#include "SocketServer.hpp"
#include <gtest/gtest.h>
#include <gmock/gmock.h>

namespace 
{
    class MockSocketImp : public Socket
    {
    public:
        MOCK_METHOD(void, m_init, (), (override));
        MOCK_METHOD(void, m_accept, (), (override));
        MOCK_METHOD(void, m_recvfrom, (::std::string &), (override));
        MOCK_METHOD(void, m_closeConnection, (), (override));
        MOCK_METHOD(void, m_closeServer, (), (override));
    };

    class MockSocketFactory : public SocketFactory
    {
    public:
        MockSocketFactory() {}
        Socket *createSocket(int port) override
        {
            return &mocksocket;
        }
        MockSocketImp mocksocket;
    };

    class SocketServerTest : public ::testing::Test
    {
    protected:
        MockSocketFactory *mockfactory;
        std::unique_ptr<::SocketServer> ss;
        SocketServerTest() {}
        ~SocketServerTest() override
        {
        }
        void SetUp() override
        {
            MockSocketImp mocksocket;
            mockfactory = new MockSocketFactory();
            ss = std::make_unique<::SocketServer>(mockfactory);
        }

        void TearDown() override
        {
            delete mockfactory;
        }
    };
}
