/**
 * @file ZedPositioner.hpp
 * @author Lizhou (lisie31s@gmail.com)
 * @brief ZedPositioner header file
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
#define ZED_FACTOR 10000    // ZED position factor

    /**
     * @brief ZedPositioner class
     * 
     * @details This class is used to get position from ZED camera with ZED SDK
     */
    class ZedPositioner : public positiontracker::IPositioner
    {
    public:

        /**
         * @brief Construct a new Zed Positioner object
         * 
         * @details This constructor is used to initialize ZED camera and enable position tracking
         */
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

        /**
         * @brief Destroy the Zed Positioner object
         * 
         * @details This destructor is used to disable position tracking and close ZED camera
         */
        ~ZedPositioner()
        {
            zed.disablePositionalTracking();
            zed.close();
        }

        /**
         * @brief Get the Status object
         * 
         * @param _status status of positioner
         */
        void getStatus(int &_status) override;

        /**
         * @brief Get the Position object
         * 
         * @param _x x position
         * @param _y y position
         * @param _z z position
         */
        void getPosition(int &_x, int &_y, int &_z) override;

    private:

        /**
         * @brief ZED camera object
         */
        sl::Camera zed;

        /**
         * @brief ZED camera pose object
         */
        sl::Pose zed_pose;

        /**
         * @brief Positioner status
         */
        std::atomic<int> status;
    };
}

#endif /* __ZEDPOSITIONER_HPP */
#endif /* DEPLOYMENT_ENV */