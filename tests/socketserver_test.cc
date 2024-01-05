#include "socketserver_test.h"
#include <iostream>
TEST_F(SocketServerTest, socketserverinitSuccess)
{
    EXPECT_CALL(getFactory()->mocksocket, m_init()).Times(1);
    EXPECT_NO_THROW(ss->init(8888));
}

TEST_F(SocketServerTest, socketserverinitFailed)
{
    EXPECT_CALL(getFactory()->mocksocket, m_init())
        .Times(1)
        .WillOnce(::testing::Throw(::SocketException("Mock Socket Error")));
    EXPECT_CALL(getFactory()->mocksocket, m_closeServer()).Times(1);

    EXPECT_NO_THROW(ss->init(8888));
}

TEST_F(SocketServerTest, socketserverwaitingconnectionSuccess)
{
    EXPECT_CALL(getFactory()->mocksocket, m_init()).Times(1);
    EXPECT_CALL(getFactory()->mocksocket, m_accept()).Times(1);
    EXPECT_NO_THROW(ss->init(8888));
    EXPECT_NO_THROW(ss->waiting_connection());
}

TEST_F(SocketServerTest, socketserverwaintingConnectionBeforeInit)
{

    EXPECT_CALL(getFactory()->mocksocket, m_accept()).Times(0);
    EXPECT_THROW({
        try
        {
            ss->waiting_connection();
        }
        catch (const SocketException &e)
        {
            EXPECT_STREQ("Faied waiting connection before init", e.what());
            throw;
        }
    },
                 SocketException);
}

TEST_F(SocketServerTest, socketserverwaitingConnectionFailed)
{
    EXPECT_CALL(getFactory()->mocksocket, m_init()).Times(1);
    EXPECT_CALL(getFactory()->mocksocket, m_accept()).Times(1).WillOnce(::testing::Throw(::SocketException("Mock Socket Error")));
    EXPECT_CALL(getFactory()->mocksocket, m_closeServer()).Times(1);

    EXPECT_NO_THROW(ss->init(8888));
    EXPECT_NO_THROW(ss->waiting_connection());
}

TEST_F(SocketServerTest, socketserverwaittingCommandSuccess)
{
    EXPECT_CALL(getFactory()->mocksocket, m_init()).Times(1);
    EXPECT_CALL(getFactory()->mocksocket, m_accept()).Times(1);
    EXPECT_CALL(getFactory()->mocksocket, m_recvfrom(::testing::_)).Times(1);
    EXPECT_NO_THROW(ss->init(8888));
    EXPECT_NO_THROW(ss->waiting_connection());
    setMessageTest();
    EXPECT_NO_THROW(ss->waiting_command());
    EXPECT_EQ("Test", getMessage());
}

TEST_F(SocketServerTest, socketserverwiattingCommandBeforeInit)
{
    EXPECT_CALL(getFactory()->mocksocket, m_accept()).Times(0);
    EXPECT_CALL(getFactory()->mocksocket, m_recvfrom(::testing::_)).Times(0);
    EXPECT_THROW({
        try
        {
            ss->waiting_command();
        }
        catch (const SocketException &e)
        {
            EXPECT_STREQ("Failed waitting command before init.", e.what());
            throw;
        }
    },
                 SocketException);
}

TEST_F(SocketServerTest, socketserver_executeCommandSuccess)
{
    EXPECT_CALL(*raw_pub, publish("GetInfo", ::testing::_)).Times(1);
    auto getInfoCMD = (testJson["GETINFO"])[0].dump();
    setMessage(getInfoCMD);
    EXPECT_NO_THROW(ss->execute_command());
    EXPECT_EQ(getMessage(), "");
}

TEST_F(SocketServerTest, socketserver_executeCommandFailed)
{
    EXPECT_CALL(*raw_pub, publish(::testing::_, ::testing::_)).Times(0);
    setMessageTest();
    EXPECT_THROW(ss->execute_command(), std::invalid_argument);
    EXPECT_EQ(getMessage(), "");
}

TEST_F(SocketServerTest, socketsever_CreateFileSuccess)
{
    callback = [this](const std::string &topic, const std::string &data)
    { ss->createFile_callback(topic, data); };
    auto createCMD = (testJson["CREATE"])[0].dump();
    setMessage(createCMD);
    EXPECT_CALL(*raw_pub, publish("CreateFile", ::testing::_)).Times(1).WillOnce(::testing::Invoke(callback));
    ss->execute_command();
    ASSERT_TRUE(fileExists(testFilePath));
}

TEST_F(SocketServerTest, socketserver_CreateFileFailed)
{
    callback = [this](const std::string &topic, const std::string &data)
    { ss->createFile_callback(topic, data); };
    auto createCMD = (testJson["CREATE"])[2].dump();
    std::string filename = createCMD;
    setMessage(filename);
    EXPECT_CALL(*raw_pub, publish("CreateFile", ::testing::_)).Times(1).WillOnce(::testing::Invoke(callback));
    EXPECT_THROW(ss->execute_command(), std::runtime_error);
}