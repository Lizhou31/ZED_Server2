#include "SocketServer.hpp"
#include <gtest/gtest.h>
#include <gmock/gmock.h>

class MockSocketImp : public Socket
{
public:
    MOCK_METHOD(void, m_init, (), (override));
    MOCK_METHOD(void, m_accept, (), (override));
    MOCK_METHOD(void, m_recvfrom, (::std::string &), (override));
    MOCK_METHOD(void, m_sendto, (::std::string &), (override));
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
    MOCK_METHOD(void, publish,
                (const std::string &topic, const std::string &data), (override));
};

class MockSubscriber : public simplepubsub::ISubscriber
{
public:
    MOCK_METHOD(void, onMessageReceived,
                (const std::function<void(const std::string &, const std::string &)> &callback), (override));
};

class MockAgent : public simplepubsub::IAgent
{
public:
    MOCK_METHOD(std::unique_ptr<simplepubsub::IPublisher>, requestPublisher, (), (override));
    MOCK_METHOD(std::unique_ptr<simplepubsub::ISubscriber>, requestSubcriber,
                (const std::string &topic, const std::function<void(const std::string &, const std::string &)> &callback), (override));
};

class SocketServerTest : public ::testing::Test
{
protected:
    std::unique_ptr<mysocketserver::SocketServer> ss;
    std::unique_ptr<::MockAgent> agent;
    std::unique_ptr<::MockPublisher> pub;
    std::function<void(const std::string &, const std::string &)> callback;
    MockPublisher *raw_pub;
    nlohmann::json testJson;
    std::string testFilePath;

    SocketServerTest() : raw_pub(nullptr) {}
    ~SocketServerTest() override
    {
    }
    void SetUp() override
    {
        /* Load the test JSON file */
        std::ifstream file("test_data/test_command.json");
        ASSERT_TRUE(file.is_open());
        std::stringstream buffer;
        buffer << file.rdbuf();
        file.close();
        testJson = nlohmann::json::parse(buffer.str());

        /* Mock the publish function */
        pub = std::make_unique<::MockPublisher>();
        raw_pub = pub.get();

        /* Mock the Agent function */
        agent = std::make_unique<::MockAgent>();

        /* Init the SocketServer */
        ss = std::make_unique<mysocketserver::SocketServer>(std::move(std::make_unique<MockSocketFactory>()),
                                                            std::move(pub));

        testFilePath = "./Data/" + (((testJson["CREATE"])[0])["Args"])[0].get<std::string>() + ".csv";
    }

    void TearDown() override
    {
        ss.reset();
        raw_pub = nullptr;
        agent.reset();
        remove(testFilePath.c_str());
    }

    ::std::string &getMessage()
    {
        return ss->message;
    }

    void setMessageTest()
    {
        ss->message = "Test";
    }

    void setMessage(const std::string &_message)
    {
        ss->message = _message;
    }

    MockSocketFactory *getFactory()
    {
        return dynamic_cast<MockSocketFactory *>((ss->factory).get());
    }

    bool fileExists(const std::string &filePath)
    {
        std::ifstream file(filePath);
        return file.good();
    }

    void createtestFile()
    {
        ss->result_file.open("./Data/test.csv", std::ios::app);
    }

    bool isFileClose()
    {
        return !ss->result_file.is_open();
    }

    std::string getFileResult()
    {
        std::ifstream file(testFilePath);
        EXPECT_TRUE(file.is_open());
        std::string result;
        std::getline(file, result);
        return result;
    }

    void set_zed_status(int data)
    {
        ss->zed_status.store(data);
    }

    const std::string pack_infoData_caller()
    {
        return ss->pack_infoData();
    }
};
