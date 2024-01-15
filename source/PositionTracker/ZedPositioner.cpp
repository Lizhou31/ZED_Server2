/**
 * @file ZedPositioner.cpp
 * @author Lizhou (lisie31s@gmail.com)
 * @brief ZedPositioner source file
 * @details This file implements the ZedPositioner class with ZED SDK
 * @version 0.1
 * @date 2024-01-12
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifdef USE_DEPLOYMENT_ENV
#include "ZedPositioner.hpp"
using namespace zedpositioner;

/**
 * @brief Get the status from ZedPositioner
 * 
 * @details This function is used to get the atomic status from the getPosition function updated.
 * 
 * @param _status reference to status return
 */
void ZedPositioner::getStatus(int &_status)
{
    _status = status.load();
}

/**
 * @brief Get the Position from ZedPositioner
 * 
 * @details This function is used to get the position and status from ZED camera with ZED SDK
 * 
 * @param _x reference to x return
 * @param _y reference to y return
 * @param _z reference to z return
 */
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