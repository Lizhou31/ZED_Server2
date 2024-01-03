#include "SocketServer.hpp"
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <fstream> 

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

class MockPublisher : public simplepubsub::IPublisher
{
public:
    MOCK_METHOD(void, publish, (const std::string &topic, const std::string &data), (override));
};

class SocketServerTest : public ::testing::Test
{
protected:
    std::unique_ptr<::SocketServer> ss;
    std::unique_ptr<::MockPublisher> pub;
    MockPublisher *raw_pub;
    std::string getinfo_cmd;
    SocketServerTest() {}
    ~SocketServerTest() override
    {
    }
    void SetUp() override
    {
        std::ifstream file("test_data/test_command.json");
        ASSERT_TRUE(file.is_open());

        std::stringstream buffer;
        buffer << file.rdbuf();
        file.close();

        auto testJson = nlohmann::json::parse(buffer.str());
        getinfo_cmd = (testJson["GETINFO"])[0].dump();

        pub = std::make_unique<::MockPublisher>();
        raw_pub = pub.get();
        ss = std::make_unique<::SocketServer>(std::move(std::make_unique<MockSocketFactory>()),
                                              std::move(pub));
    }

    void TearDown() override
    {
        ss.reset();
        raw_pub = nullptr;
    }

    ::std::string &getMessage()
    {
        return ss->message;
    }

    void setMessageTest()
    {
        ss->message = "Test";
    }

    void setMessage(std::string &_message){
        ss->message = _message;
    }

    MockSocketFactory *getFactory()
    {
        return dynamic_cast<MockSocketFactory *>((ss->factory).get());
    }
};
