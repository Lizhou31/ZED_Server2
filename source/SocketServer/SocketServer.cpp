#include "SocketServer.hpp"

void SocketServer::init(int port)
{
    
}

void SocketServer::shutdown()
{
    if (socket)
    {
        try
        {
            socket->close();
        }
        catch (const SocketException &e)
        {
            std::cerr << "Socket error: " << e.what() << std::endl;
            throw e;
        }
        delete socket;
    }
}
