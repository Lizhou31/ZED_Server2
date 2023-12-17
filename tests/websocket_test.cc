#include "websocket_test.h"
namespace
{
    TEST_F(WebSocketTest, testinitSuccess)
    {
        EXPECT_CALL(mockSys, Cbind(::testing::_, ::testing::_, ::testing::_)).Times(1).WillOnce(::testing::Return(0));
        EXPECT_CALL(mockSys, Clisten(::testing::_, 1)).Times(1).WillOnce(::testing::Return(0));

        EXPECT_NO_THROW(ws->init());
    }

    TEST_F(WebSocketTest, testinitFailedBind)
    {
        EXPECT_CALL(mockSys, Cbind(::testing::_, ::testing::_, ::testing::_)).Times(1).WillOnce(::testing::Return(-1));
        
        EXPECT_THROW({
            try
            {
                ws->init();
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
                ws->init();
            }
            catch (const SocketException &e)
            {
                EXPECT_STREQ("Socket listen error", e.what());
                throw;
            }
        },
                     SocketException);
    }

}
