#include "PositionTracker.hpp"
#include <iostream>
using namespace positiontracker;

void PositionTracker::getData()
{
    while (keepRunning)
    {
        positioner->getStatus(status);
        pub_ptr->publish("ZED_Status", packStatus());
        positioner->getPosition(position.x, position.y, position.z);
        pub_ptr->publish("ZED_Position", packPosition());
    }
}

const std::string PositionTracker::packStatus()
{
    nlohmann::json stat;
    stat["Status"] = status;
    return nlohmann::to_string(stat);
}

const std::string PositionTracker::packPosition()
{
    nlohmann::json pos;
    pos["X"] = position.x;
    pos["Y"] = position.y;
    pos["Z"] = position.z;
    return nlohmann::to_string(pos);
}
