/**
 * @file ZedPositioner.cpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-01-12
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifdef USE_DEPLOYMENT_ENV
#include "ZedPositioner.hpp"
using namespace zedpositioner;

void ZedPositioner::getStatus(int &_status)
{
    _status = status.load();
}

void ZedPositioner::getPosition(int &_x, int &_y, int &_z)
{
    if (zed.grab() == sl::ERROR_CODE::SUCCESS)
    {
        sl::POSITIONAL_TRACKING_STATE state = zed.getPosition(zed_pose, sl::REFERENCE_FRAME::WORLD);
        if (state == sl::POSITIONAL_TRACKING_STATE::OK)
        {
            status.store(positiontracker::Positioner_Status::OK);
        }
        else if (state == sl::POSITIONAL_TRACKING_STATE::SEARCHING)
        {
            status.store(positiontracker::Positioner_Status::Searching);
        }
        _x = int(zed_pose.getTranslation().tx * ZED_FACTOR);
        _y = int(zed_pose.getTranslation().ty * ZED_FACTOR);
        _z = int(zed_pose.getTranslation().tz * ZED_FACTOR);
    }
}

#endif /* DEPLOYMENT_ENV */