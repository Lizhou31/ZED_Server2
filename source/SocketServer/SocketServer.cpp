/**
 * @file SocketServer.cpp
 * @author Lizhou (lisie31s@gmail.com)
 * @brief Socket server implementation
 * @version 0.1
 * @date 2024-01-15
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "SocketServer.hpp"

using namespace mysocketserver;

/**
 * @brief SocketServer initialize function
 *
 * @details This function will create a socket and bind it to the port.
 *
 * @param port socket port listening
 */
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

/**
 * @brief SocketServer waiting connection function
 *
 * @details This function will waiting for connection. Close the server if failed.
 */
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

/**
 * @brief SocketServer waiting command function
 *
 * @details This function will waiting for command. Close the client and reset the server parameters if failed.
 */
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
        std::cerr << "Error occurred while receive"
                  << " " << e.what() << std::endl;
        result_file.close();
        pt.reset();
        zed_setZero();
        throw std::runtime_error("Waiting command error.");
    }
}

/**
 * @brief SocketServer execute command function
 *
 * @details This function will execute the command. Clear the message if failed.
 */
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

/**
 * @brief SocketServer shutdown function
 *
 * @todo Add test
 */
void SocketServer::shutdown()
{
    // TODO: Add test
    result_file.close();
    message.clear();
    pt.reset();
    zed_setZero();
}

/**
 * @brief CREATE command callback function
 *
 * @details This function will create a file and initialize the position tracker.
 *          Position tracker will start a thread to publish the position.
 *
 * @param topic subscription topic
 * @param data subscription data in json format
 *
 * @todo Refactor: might need to saperate the start and create file.
 * @todo Refactor: might need to saperate the position tracker.
 * @todo CreateFile while the directory doesn't exist?
 */
void SocketServer::createFile_callback(const std::string &topic, const std::string &data)
{
    auto file_name = (nlohmann::json::parse(data)["id"]).get<std::string>();
    result_file.open("./Data/" + file_name + ".csv", std::ios::app);
    if (result_file.fail())
    {
        throw std::runtime_error("Failed to open file: " + file_name);
    }
#ifdef USE_DEPLOYMENT_ENV
    pt = std::make_unique<positiontracker::PositionTracker>(positioner_pub, std::make_unique<zedpositioner::ZedPositioner>());
    // TODO: Refactor, and test the positioner.
#endif
    // TODO: CreateFile while the directory doesn't exist?
    // TODO: Refactor: might need to saperate the start and create file.
}

/**
 * @brief STOP command callback function
 *
 * @details This function will close the file and reset the position tracker.
 *
 * @param topic subscription topic
 * @param data subscription data in json format
 */
void SocketServer::stop_callback(const std::string &topic, const std::string &data)
{
    result_file.close();
    zed_setZero();
    pt.reset();
}

/**
 * @brief Probe command callback function
 *
 * @details This function will write the probe data to the file.
 *
 * @param topic subscription topic
 * @param data subscription data in json format
 */
void SocketServer::probe_callback(const std::string &topic, const std::string &data)
{
    result_file << data << ",";
    result_file << float(zed_x.load()) / ZED_FACTOR << ",";
    result_file << float(zed_y.load()) / ZED_FACTOR << ",";
    result_file << float(zed_z.load()) / ZED_FACTOR << std::endl;
}

/**
 * @brief GetInfo command callback function
 *
 * @details This function will pack the info data and send it to the client.
 *
 * @param topic subscription topic
 * @param data subscription data in json format
 */
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

/**
 * @brief ZED_Status command callback function
 *
 * @details This function will update the ZED status.
 *
 * @param topic subscription topic
 * @param data subscription data in json format
 */
void SocketServer::zedStatus_callback(const std::string &topic, const std::string &data)
{
    nlohmann::json status = nlohmann::json::parse(data);
    zed_status.store(status["Status"].get<int>());
}

/**
 * @brief ZED_Position command callback function
 *
 * @details This function will update the ZED position.
 *
 * @param topic subscription topic
 * @param data subscription data in json format
 */
void SocketServer::zedPosition_callback(const std::string &topic, const std::string &data)
{
    nlohmann::json position = nlohmann::json::parse(data);
    zed_x.store(position["X"].get<int>());
    zed_y.store(position["Y"].get<int>());
    zed_z.store(position["Z"].get<int>());
}

/**
 * @brief Register the subscriber
 *
 * @details This function will register the subscribers.
 *
 * @param agent The agent of simplepubsub
 */
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
    topic_status = agent.requestSubcriber("ZED_Status", [this](const std::string &topic, const std::string &data)
                                          { zedStatus_callback(topic, data); });
    topic_position = agent.requestSubcriber("ZED_Position", [this](const std::string &topic, const std::string &data)
                                            { zedPosition_callback(topic, data); });
}