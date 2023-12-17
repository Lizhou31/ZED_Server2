#include "websocket_test.h"
namespace 
{
    TEST_F(WebSocketTest, testinitSuccess)
    {
        int server_fd;
        struct sockaddr_in sockaddr;
        getCbindParam(&server_fd, &sockaddr);
        EXPECT_CALL(mockSys, Cbind(server_fd, (struct sockaddr*) &sockaddr, sizeof(sockaddr))).WillOnce(::testing::Return(0));
        EXPECT_CALL(mockSys, Clisten(server_fd, 1)).WillOnce(::testing::Return(0));

        EXPECT_NO_THROW(ws->init());
    }
}
