#include "SocketServer.hpp"

void SocketServer::init(int port)
{
    socket = factory->createSocket(port);
    try
    {
        socket->m_init();
    }
    catch (SocketException &e)
    {
        std::cerr << "Error occurred: " << e.what() << std::endl;
        socket->m_closeServer();
    }
}

void SocketServer::waiting_connection()
{
    if (!socket)
    {
        throw SocketException("Faied waiting connection before init");
    }
    try
    {
        socket->m_accept();
    }
    catch (SocketException &e)
    {
        std::cerr << "Error occurred: " << e.what() << std::endl;
        socket->m_closeServer();
    }
}

void SocketServer::waiting_command()
{
    if (!socket)
    {
        throw SocketException("Failed waitting command before init.");
    }
    try
    {
        socket->m_recvfrom(message);
    }
    catch (SocketException &e)
    {
        std::cerr << "Error occurred while receive" << e.what() << std::endl;
        throw std::runtime_error("Waiting command error.");
    }
}

void SocketServer::execute_command()
{
    try
    {
        invoker.executeCommand(message);
    }
    catch (std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        message.clear();
        throw;
    }
    message.clear();
}

void SocketServer::createFile_callback(const std::string &topic, const std::string &data)
{
    auto file_name = (nlohmann::json::parse(data)["id"]).get<std::string>();
    result_file.open("./Data/" + file_name + ".csv", std::ios::app);
    if (result_file.fail())
    {
        throw std::runtime_error("Failed to open file: " + file_name);
    }
    // TODO: Start the ZED server
    // TODO: CreateFile while the directory doesn't exist?
    // TODO: Refactor: might need to saperate the start and create file.
}

void SocketServer::stop_callback(const std::string &topic, const std::string &data)
{
    result_file.close();
    // TODO: Stop the ZedServer.
}
void SocketServer::probe_callback(const std::string &topic, const std::string &data)
{
    
    // TODO: save the data
}

void SocketServer::shutdown()
{
    result_file.close();
}
