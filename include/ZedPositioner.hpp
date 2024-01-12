/**
 * @file ZedPositioner.hpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-01-12
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifdef USE_DEPLOYMENT_ENV
#ifndef __ZEDPOSITIONER_HPP
#define __ZEDPOSITIONER_HPP

#include <sl/Camera.hpp>
#include "PositionTracker.hpp"

namespace zedpositioner
{
#define ZED_FACTOR 10000

    class ZedPositioner : public positiontracker::IPositioner
    {
    public:
        ZedPositioner() : status(positiontracker::Positioner_Status::Stop)
        {
            sl::InitParameters init_params;
            init_params.camera_resolution = sl::RESOLUTION::HD720;                    // Use HD720 video mode (default fps: 60)
            init_params.coordinate_system = sl::COORDINATE_SYSTEM::RIGHT_HANDED_Y_UP; // Use a right-handed Y-up coordinate system
            init_params.coordinate_units = sl::UNIT::METER;                           // Set units in meters

            auto returned_state = zed.open(init_params);
            if (returned_state != sl::ERROR_CODE::SUCCESS)
            {
                throw std::system_error(EBUSY, std::generic_category(), "Camera open failed");
            }

            sl::PositionalTrackingParameters tracking_parameters;

            returned_state = zed.enablePositionalTracking(tracking_parameters);
            if (returned_state != sl::ERROR_CODE::SUCCESS)
            {
                zed.close();
                throw std::system_error(EBUSY, std::generic_category(), "Position tracking enable error");
            }
        }
        ~ZedPositioner()
        {
            zed.disablePositionalTracking();
            zed.close();
        }
        void getStatus(int &) override;
        void getPosition(int &, int &, int &) override;

    private:
        sl::Camera zed;
        sl::Pose zed_pose;
        std::atomic<int> status;
    };
}

#endif /* __ZEDPOSITIONER_HPP */
#endif /* DEPLOYMENT_ENV */