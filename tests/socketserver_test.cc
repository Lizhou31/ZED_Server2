/**
 * @file socketserver_test.cc
 * @author Lizhou (lisie31s@gmail.com)
 * @brief socket server test implementation
 * @version 0.1
 * @date 2024-01-16
 * 
 * @copyright Copyright (c) 2024
 */

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

TEST_F(SocketServerTest, socketserverwaitingCommandSuccess)
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

TEST_F(SocketServerTest, socketserverwaitingCommandFailed)
{
    EXPECT_CALL(getFactory()->mocksocket, m_init()).Times(1);
    EXPECT_CALL(getFactory()->mocksocket, m_accept()).Times(1);
    EXPECT_CALL(getFactory()->mocksocket, m_recvfrom(::testing::_)).Times(1).WillOnce([this]()
                                                                                      { throw SocketException("test"); });
    EXPECT_NO_THROW(ss->init(8888));
    EXPECT_NO_THROW(ss->waiting_connection());

    EXPECT_THROW({
        try
        {
            ss->waiting_command();
        }
        catch (const std::runtime_error &e)
        {
            EXPECT_STREQ("Waiting command error.", e.what());
            throw;
        }
    },
                 std::runtime_error);
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
    EXPECT_CALL(*raw_pub, publish(::testing::_, ::testing::_)).WillRepeatedly(::testing::Invoke(callback));
    EXPECT_NO_THROW(ss->execute_command());
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

TEST_F(SocketServerTest, socketserver_StopSuccess)
{
    createtestFile();
    ASSERT_TRUE(fileExists(testFilePath));

    callback = [this](const std::string &topic, const std::string &data)
    { ss->stop_callback(topic, data); };
    auto stopCMD = (testJson["STOP"])[0].dump();
    setMessage(stopCMD);
    EXPECT_CALL(*raw_pub, publish("StopTest", ::testing::_)).Times(1).WillOnce(::testing::Invoke(callback));
    EXPECT_NO_THROW(ss->execute_command());

    EXPECT_TRUE(isFileClose());
}

TEST_F(SocketServerTest, socketserver_ProbeSuccess)
{
    createtestFile();
    ASSERT_TRUE(fileExists(testFilePath));

    callback = [this](const std::string &topic, const std::string &data)
    { ss->probe_callback(topic, data); };
    auto probeCMD = (testJson["PROBE"])[0].dump();
    setMessage(probeCMD);
    EXPECT_CALL(*raw_pub, publish("Probe", "123")).Times(1).WillOnce(::testing::Invoke(callback));
    EXPECT_NO_THROW(ss->execute_command());

    EXPECT_EQ("123,0,0,0", getFileResult());
}

TEST_F(SocketServerTest, socketserver_packinfoData)
{
    set_zedStatus(0);
    const std::string result = pack_infoData_caller();
    auto returnResult = (testJson["RETURNINFO"])[0].dump();
    EXPECT_EQ(returnResult, result);
}

TEST_F(SocketServerTest, socketserver_getInfoSuccess)
{
    EXPECT_CALL(getFactory()->mocksocket, m_init()).Times(1);
    EXPECT_CALL(getFactory()->mocksocket, m_accept()).Times(1);
    EXPECT_NO_THROW(ss->init(8888));
    EXPECT_NO_THROW(ss->waiting_connection());

    callback = [this](const std::string &topic, const std::string &data)
    { ss->getInfo_callback(topic, data); };

    auto getInfoCMD = ((testJson["GETINFO"])[0]).dump();
    setMessage(getInfoCMD);
    EXPECT_CALL(*raw_pub, publish("GetInfo", ::testing::_)).Times(1).WillOnce(::testing::Invoke(callback));
    set_zedStatus(0);
    std::string result = pack_infoData_caller();

    EXPECT_CALL(getFactory()->mocksocket, m_sendto(result)).Times(1);
    EXPECT_NO_THROW(ss->execute_command());
}

TEST_F(SocketServerTest, socketserver_getInfoFailed)
{
    EXPECT_CALL(getFactory()->mocksocket, m_init()).Times(1);
    EXPECT_CALL(getFactory()->mocksocket, m_accept()).Times(1);
    EXPECT_NO_THROW(ss->init(8888));
    EXPECT_NO_THROW(ss->waiting_connection());

    callback = [this](const std::string &topic, const std::string &data)
    { ss->getInfo_callback(topic, data); };

    auto getInfoCMD = ((testJson["GETINFO"])[0]).dump();
    setMessage(getInfoCMD);
    EXPECT_CALL(*raw_pub, publish("GetInfo", ::testing::_)).Times(1).WillOnce(::testing::Invoke(callback));
    set_zedStatus(0);
    std::string result = pack_infoData_caller();

    EXPECT_CALL(getFactory()->mocksocket, m_sendto(result)).Times(1).WillOnce(::testing::Throw(SocketException("test sendto error")));
    EXPECT_NO_THROW(ss->execute_command());
}

TEST_F(SocketServerTest, socketserver_zedStatusSuccess)
{
    nlohmann::json status;
    status["Status"] = 1;
    EXPECT_NO_THROW(ss->zedStatus_callback("ZED_Status", nlohmann::to_string(status)));
    EXPECT_EQ(1, get_zedStatus());
}

TEST_F(SocketServerTest, socketserver_zedStatusFailed)
{
    // TODO: TestFaied
}

TEST_F(SocketServerTest, socketserver_zedPositionSuccess)
{
    nlohmann::json position;
    position["X"] = 1;
    position["Y"] = 2;
    position["Z"] = 3;
    EXPECT_NO_THROW(ss->zedPosition_callback("ZED_Position", nlohmann::to_string(position)));

    EXPECT_EQ(1, get_zedX());
    EXPECT_EQ(2, get_zedY());
    EXPECT_EQ(3, get_zedZ());
}

TEST_F(SocketServerTest, socketserver_zedPositionFailed)
{
    // TODO: TestFailed
}

TEST_F(SocketServerTest, socketserver_registerSubscriber)
{
    // TODO: Hasn't test the subscriber unique_ptr allocate properly.

    EXPECT_CALL(*agent, requestSubcriber("CreateFile", ::testing::_)).Times(1).WillOnce(::testing::Invoke([this](const std::string &arg1, std::function<void(const std::string &, const std::string &)> arg2) -> std::unique_ptr<simplepubsub::ISubscriber>
                                                                                                          { return std::make_unique<MockSubscriber>(); }));

    EXPECT_CALL(*agent, requestSubcriber("Probe", ::testing::_)).Times(1).WillOnce(::testing::Invoke([this](const std::string &arg1, std::function<void(const std::string &, const std::string &)> arg2) -> std::unique_ptr<simplepubsub::ISubscriber>
                                                                                                     { return std::make_unique<MockSubscriber>(); }));

    EXPECT_CALL(*agent, requestSubcriber("StopTest", ::testing::_)).Times(1).WillOnce(::testing::Invoke([this](const std::string &arg1, std::function<void(const std::string &, const std::string &)> arg2) -> std::unique_ptr<simplepubsub::ISubscriber>
                                                                                                        { return std::make_unique<MockSubscriber>(); }));

    EXPECT_CALL(*agent, requestSubcriber("GetInfo", ::testing::_)).Times(1).WillOnce(::testing::Invoke([this](const std::string &arg1, std::function<void(const std::string &, const std::string &)> arg2) -> std::unique_ptr<simplepubsub::ISubscriber>
                                                                                                       { return std::make_unique<MockSubscriber>(); }));

    EXPECT_CALL(*agent, requestSubcriber("ZED_Status", ::testing::_)).Times(1).WillOnce(::testing::Invoke([this](const std::string &arg1, std::function<void(const std::string &, const std::string &)> arg2) -> std::unique_ptr<simplepubsub::ISubscriber>
                                                                                                          { return std::make_unique<MockSubscriber>(); }));

    EXPECT_CALL(*agent, requestSubcriber("ZED_Position", ::testing::_)).Times(1).WillOnce(::testing::Invoke([this](const std::string &arg1, std::function<void(const std::string &, const std::string &)> arg2) -> std::unique_ptr<simplepubsub::ISubscriber>
                                                                                                            { return std::make_unique<MockSubscriber>(); }));

    EXPECT_NO_THROW(ss->register_subscriber(*agent));
}