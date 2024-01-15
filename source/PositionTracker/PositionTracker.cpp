/**
 * @file PositionTracker.cpp
 * @author Lizhou (lisie31s@gmail.com)
 * @brief PositionTracker source file
 * @version 0.1
 * @date 2024-01-15
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "PositionTracker.hpp"
#include <iostream>
using namespace positiontracker;

/**
 * @brief getData thread function
 * 
 * @details This thread function is used to get data from positioner periodically
 */
void PositionTracker::getData()
{
    while (keepRunning)
    {
        positioner->getPosition(position.x, position.y, position.z);
        pub_ptr->publish("ZED_Position", packPosition());
        positioner->getStatus(status);
        pub_ptr->publish("ZED_Status", packStatus());
    }
    status = Positioner_Status::Stop;
    pub_ptr->publish("ZED_Status", packStatus());
}

/**
 * @brief packStatus function
 * 
 * @return const std::string formatted status
 * 
 * @details This function is used to pack the status object in json format
 */
const std::string PositionTracker::packStatus()
{
    nlohmann::json stat;
    stat["Status"] = status;
    return nlohmann::to_string(stat);
}

/**
 * @brief packPosition function
 * 
 * @return const std::string formatted position
 * 
 * @details This function is used to pack the position object in json format
 */
const std::string PositionTracker::packPosition()
{
    nlohmann::json pos;
    pos["X"] = position.x;
    pos["Y"] = position.y;
    pos["Z"] = position.z;
    return nlohmann::to_string(pos);
}
