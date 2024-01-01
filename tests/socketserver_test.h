#include "SocketServer.hpp"
#include <gtest/gtest.h>
#include <gmock/gmock.h>

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
    std::unique_ptr<simplepubsub::Agent> agent;
    std::unique_ptr<::SocketServer> ss;
    SocketServerTest() {}
    ~SocketServerTest() override
    {
    }
    void SetUp() override
    {
        agent = std::make_unique<simplepubsub::Agent>();
        auto ptr = agent->requestPublisher();
        ss = std::make_unique<::SocketServer>(std::move(std::make_unique<MockSocketFactory>()),
                                              std::move(ptr));
    }

    void TearDown() override
    {
        ss.reset();
        agent.reset();
    }

    ::std::string &getMessage()
    {
        return ss->message;
    }

    void setMessageTest()
    {
        ss->message = "Test";
    }

    MockSocketFactory *getFactory()
    {
        return dynamic_cast<MockSocketFactory*>((ss->factory).get());
    }
};
