#include "socketserver_test.h"

TEST_F(SocketServerTest, socketserverinitSuccess)
{
    EXPECT_CALL(mockfactory->mocksocket, m_init()).Times(1);
    EXPECT_NO_THROW(ss->init(8888));
}

TEST_F(SocketServerTest, socketserverinitFailed)
{
    EXPECT_CALL(mockfactory->mocksocket, m_init())
        .Times(1)
        .WillOnce(::testing::Throw(::SocketException("Mock Socket Error")));
    EXPECT_CALL(mockfactory->mocksocket, m_closeServer()).Times(1);

    EXPECT_NO_THROW(ss->init(8888));
}

TEST_F(SocketServerTest, socketserverwaitingconnectionSuccess)
{
    EXPECT_CALL(mockfactory->mocksocket, m_init()).Times(1);
    EXPECT_CALL(mockfactory->mocksocket, m_accept()).Times(1);
    EXPECT_NO_THROW(ss->init(8888));
    EXPECT_NO_THROW(ss->waiting_connection());
}

TEST_F(SocketServerTest, socketserverwaintingbeforeinit)
{

    EXPECT_CALL(mockfactory->mocksocket, m_accept()).Times(0);
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

TEST_F(SocketServerTest, socketserverwaitingconnectionFailed)
{
    EXPECT_CALL(mockfactory->mocksocket, m_init()).Times(1);
    EXPECT_CALL(mockfactory->mocksocket, m_accept()).Times(1)
        .WillOnce(::testing::Throw(::SocketException("Mock Socket Error")));
    EXPECT_CALL(mockfactory->mocksocket, m_closeServer()).Times(1);

    EXPECT_NO_THROW(ss->init(8888));
    EXPECT_NO_THROW(ss->waiting_connection());
}
