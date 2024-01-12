#include <iostream>
#include "SocketServer.hpp"
#include "WebSocket.hpp"
#include "SimplePubSub.hpp"

int main()
{
    simplepubsub::Agent agent;

    mysocketserver::SocketServer server(std::move(std::make_unique<myWebSocket::WebSocketFactory>()),
                        std::move(agent.requestPublisher()));
    server.register_subscriber(agent);
    server.init(12002);

WAIT:
    server.waiting_connection();        // waiting connection

    while (true)
    {
        try
        {
            server.waiting_command();
        }
        catch (std::exception &e)
        {
            std::cout << e.what() << std::endl;
            goto WAIT;
        }
        try
        {
            server.execute_command();
        }
        catch (std::exception &e)
        {
            std::cout << e.what() << std::endl;
        }
    }

    
}