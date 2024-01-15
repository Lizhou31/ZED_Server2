/**
 * @file CommandSolver.cpp
 * @author Lizhou31 (lisie31s@gmail.com)
 * @brief A CommandSolver class implementation
 * @version 0.1
 * @date 2024-01-15
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "CommandSolver.hpp"

using namespace commandsolver;
using namespace nlohmann;

/**
 * @brief Execute the command (CREATE)
 * 
 * @param ptr publisher smart pointer
 */
void CreateCommand::execute(std::shared_ptr<simplepubsub::IPublisher> ptr)
{
    ptr->publish("CreateFile", json{{"id", data.id},
                                    {"redius", data.redius},
                                    {"height", data.height}}
                                   .dump());
}

/**
 * @brief Execute the command (PROBE)
 * 
 * @param ptr publisher smart pointer
 */
void ProbeCommand::execute(std::shared_ptr<simplepubsub::IPublisher> ptr)
{
    ptr->publish("Probe", sensor_data);
}

/**
 * @brief Execute the command (STOP)
 * 
 * @param ptr publisher smart pointer
 */
void StopCommand::execute(std::shared_ptr<simplepubsub::IPublisher> ptr)
{
    ptr->publish("StopTest", "");
}

/**
 * @brief Execute the command (GETINFO)
 * 
 * @param ptr publisher smart pointer
 */
void GetInfoCommand::execute(std::shared_ptr<simplepubsub::IPublisher> ptr)
{
    ptr->publish("GetInfo", "");
}
