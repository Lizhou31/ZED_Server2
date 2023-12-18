#include "socketserver_test.h"

TEST_F(SocketServerTest, socketserverinitSuccess)
{
    EXPECT_CALL(mockfactory->mocksocket, init()).Times(1);
    EXPECT_NO_THROW(ss->init(8888));
}

TEST_F(SocketServerTest, socketserverinitFailed)
{
    EXPECT_CALL(mockfactory->mocksocket, init())
        .Times(1)
        .WillOnce(::testing::Throw(::SocketException("Mock Sock Error")));
    EXPECT_CALL(mockfactory->mocksocket, closeServer()).Times(1);

    EXPECT_NO_THROW(ss->init(8888));
}

