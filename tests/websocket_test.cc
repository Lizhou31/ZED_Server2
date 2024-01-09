#include "websocket_test.h"
namespace
{
    TEST_F(WebSocketTest, testinitSuccess)
    {
        EXPECT_CALL(mockSys, Cbind(::testing::_, ::testing::_, ::testing::_)).Times(1).WillOnce(::testing::Return(0));
        EXPECT_CALL(mockSys, Clisten(::testing::_, 1)).Times(1).WillOnce(::testing::Return(0));

        EXPECT_NO_THROW(ws->m_init());
    }

    TEST_F(WebSocketTest, testinitFailedBind)
    {
        EXPECT_CALL(mockSys, Cbind(::testing::_, ::testing::_, ::testing::_)).Times(1).WillOnce(::testing::Return(-1));

        EXPECT_THROW({
            try
            {
                ws->m_init();
            }
            catch (const SocketException &e)
            {
                EXPECT_STREQ("Socket bind error", e.what());
                throw;
            }
        },
                     SocketException);
    }

    TEST_F(WebSocketTest, testinitFailedListen)
    {
        EXPECT_CALL(mockSys, Cbind(::testing::_, ::testing::_, ::testing::_)).Times(1).WillOnce(::testing::Return(0));
        EXPECT_CALL(mockSys, Clisten(::testing::_, 1)).Times(1).WillOnce(::testing::Return(-1));

        EXPECT_THROW({
            try
            {
                ws->m_init();
            }
            catch (const SocketException &e)
            {
                EXPECT_STREQ("Socket listen error", e.what());
                throw;
            }
        },
                     SocketException);
    }

    TEST_F(WebSocketTest, testacceptSuccessAccept)
    {
        EXPECT_CALL(mockSys, Caccept(::testing::_, ::testing::_, ::testing::_)).Times(1).WillOnce(::testing::Return(888));
        EXPECT_NO_THROW(ws->m_accept());
        EXPECT_EQ(getclientfd(), 888);
    }

    TEST_F(WebSocketTest, testacceptFailedAccept)
    {
        EXPECT_CALL(mockSys, Caccept(::testing::_, ::testing::_, ::testing::_)).Times(1).WillOnce(::testing::Return(-1));
        EXPECT_THROW({
            try
            {
                ws->m_accept();
            }
            catch (const SocketException &e)
            {
                EXPECT_STREQ("Socket accept error", e.what());
                throw;
            }
        },
                     SocketException);
    }

    TEST_F(WebSocketTest, testrecvfromSuccess)
    {
        char *buf_p = getbuffer_p();
        ::std::string recv_message;
        EXPECT_CALL(mockSys, Crecvfrom(::testing::_, buf_p,
                                       2048, 0, ::testing::_, ::testing::_))
            .WillOnce(::testing::Return(10));

        // Set the buffer with test data before calling recvfrom
        const char *testData = "TTTTTTTTTT";
        memcpy(buf_p, testData, 10); // Copy test data to the buffer

        EXPECT_NO_THROW(ws->m_recvfrom(recv_message));

        EXPECT_EQ(recv_message, "TTTTTTTTTT");
    }

    TEST_F(WebSocketTest, testrecvfromFailed)
    {
        char *buf_p = getbuffer_p();
        ::std::string recv_message;
        EXPECT_CALL(mockSys, Crecvfrom(::testing::_, buf_p,
                                       2048, 0, ::testing::_, ::testing::_))
            .WillOnce(::testing::Return(-1));

        EXPECT_THROW({
            try
            {
                ws->m_recvfrom(recv_message);
            }
            catch (const SocketException &e)
            {
                EXPECT_STREQ("Socket recvfrom error", e.what());
                throw;
            }
        },
                     SocketException);
    }

    TEST_F(WebSocketTest, testsendtoSuccess)
    {
        std::string testData = "TTTTTTTTTT";
        EXPECT_CALL(mockSys, Csendto(::testing::_, (void *)testData.c_str(), testData.length(),
                                     ::testing::_, ::testing::_, ::testing::_))
            .Times(1)
            .WillOnce(::testing::Return(testData.length()));
        EXPECT_NO_THROW(ws->m_sendto(testData));
    }

    TEST_F(WebSocketTest, testsendtoFailed)
    {
        std::string testData = "TTTTTTTTTT";
        EXPECT_CALL(mockSys, Csendto(::testing::_, (void *)testData.c_str(), testData.length(),
                                     ::testing::_, ::testing::_, ::testing::_))
            .Times(1)
            .WillOnce(::testing::Return(-1));

        EXPECT_THROW({
            try
            {
                ws->m_sendto(testData);
            }
            catch (const SocketException &e)
            {
                EXPECT_STREQ("Socket sendto error", e.what());
                throw;
            }
        },
                     SocketException);
    }

    TEST_F(WebSocketTest, testcloseConnectionSuccess)
    {
        EXPECT_CALL(mockSys, Cclose(::testing::_)).WillOnce(::testing::Return(0));
        EXPECT_NO_THROW(ws->m_closeConnection());
    }

    TEST_F(WebSocketTest, testcloseConnectionFailed)
    {
        EXPECT_CALL(mockSys, Cclose(::testing::_)).WillOnce(::testing::Return(-1));
        EXPECT_THROW({
            try
            {
                ws->m_closeConnection();
            }
            catch (const SocketException &e)
            {
                EXPECT_STREQ("Socket connection close error", e.what());
                throw;
            }
        },
                     SocketException);
    }

    TEST_F(WebSocketTest, testcloseServerSuccess)
    {
        EXPECT_CALL(mockSys, Cclose(::testing::_)).WillOnce(::testing::Return(0));
        EXPECT_NO_THROW(ws->m_closeServer());
    }

    TEST_F(WebSocketTest, testcloseServerFailed)
    {
        EXPECT_CALL(mockSys, Cclose(::testing::_)).WillOnce(::testing::Return(-1));
        EXPECT_THROW({
            try
            {
                ws->m_closeServer();
            }
            catch (const SocketException &e)
            {
                EXPECT_STREQ("Socket server close error", e.what());
                throw;
            }
        },
                     SocketException);
    }
}
