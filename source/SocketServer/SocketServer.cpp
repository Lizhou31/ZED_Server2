#include "SocketServer.hpp"

using namespace mysocketserver;

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
        result_file.close();
        zed_setZero();
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
    zed_setZero();

    // TODO: Stop the ZedServer.
}
void SocketServer::probe_callback(const std::string &topic, const std::string &data)
{
    result_file << data << std::endl;
    // TODO: save the location data
}

void SocketServer::getInfo_callback(const std::string &topic, const std::string &data)
{
    std::string _message = pack_infoData();
    try
    {
        socket->m_sendto(_message);
    }
    catch (SocketException &e)
    {
        std::cerr << e.what() << std::endl;
    }
}

void SocketServer::zedStatus_callback(const std::string &topic, const std::string &data)
{
    nlohmann::json status = nlohmann::json::parse(data);
    zed_status.store(status["Status"].get<int>());
}
void SocketServer::zedPosition_callback(const std::string &topic, const std::string &data)
{
    nlohmann::json position = nlohmann::json::parse(data);
    zed_x.store(position["X"].get<int>());
    zed_y.store(position["Y"].get<int>());
    zed_z.store(position["Z"].get<int>());
}

void SocketServer::register_subscriber(simplepubsub::IAgent &agent)
{
    topic_create = agent.requestSubcriber("CreateFile", [this](const std::string &topic, const std::string &data)
                                          { createFile_callback(topic, data); });
    topic_probe = agent.requestSubcriber("Probe", [this](const std::string &topic, const std::string &data)
                                         { probe_callback(topic, data); });
    topic_stop = agent.requestSubcriber("StopTest", [this](const std::string &topic, const std::string &data)
                                        { stop_callback(topic, data); });
    topic_getInfo = agent.requestSubcriber("GetInfo", [this](const std::string &topic, const std::string &data)
                                           { getInfo_callback(topic, data); });
    topic_getInfo = agent.requestSubcriber("ZED_Status", [this](const std::string &topic, const std::string &data)
                                           { zedStatus_callback(topic, data); });
    topic_getInfo = agent.requestSubcriber("ZED_Position", [this](const std::string &topic, const std::string &data)
                                           { zedPosition_callback(topic, data); });
}

void SocketServer::shutdown()
{
    // TODO: Test
    result_file.close();
    message.clear();
    zed_setZero();
}