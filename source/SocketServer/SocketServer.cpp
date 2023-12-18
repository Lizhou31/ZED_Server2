#include "SocketServer.hpp"

void SocketServer::init(int port)
{
    socket = factory->createSocket(port);
    try{
        socket->init();
    }
    catch(SocketException &e){
        std::cerr << "Error occurred: " << e.what() << std::endl;
        socket->closeServer();
    }
}

void waiting_connection(){

}

void SocketServer::shutdown()
{
    
}
