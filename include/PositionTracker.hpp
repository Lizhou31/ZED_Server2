/**
 * @file PositionTracker.hpp
 * @author Lizhou (lisie31s@gmail.com)
 * @brief PositionTracker header file
 * @version 0.1
 * @date 2024-01-10
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef __POSITIONTRACKER_H
#define __POSITIONTRACKER_H
#include <atomic>
#include <thread>
#include "nlohmann/json.hpp"
#include "SimplePubSub.hpp"
class PositionTrackerTest;

/**
 * @namespace positiontracker
 *
 * @brief PositionTracker namespace
 */
namespace positiontracker
{
    /**
     * @brief Positioner status enum
=     */
    enum Positioner_Status
    {
        OK,
        Searching,
        Save,
        Stop
    };

    /**
     * @class IPositioner
     *
     * @brief IPositioner interface
     */
    class IPositioner
    {
    public:
        /**
         * @brief Destroy the IPositioner object
         */
        virtual ~IPositioner(){};

        /**
         * @brief Get the Status object
         *
         * @param status reference to status return
         */
        virtual void getStatus(int &status) = 0;

        /**
         * @brief Get the Position object
         * 
         * @param x reference to x return
         * @param y reference to y return
         * @param z reference to z return
         */
        virtual void getPosition(int &x, int &y, int &z) = 0;
    };

    /**
     * @class PositionTracker
     *
     * @brief PositionTracker class
     * 
     * @details This class is used to track the position of the robot, and it will continuously publish the position and status to the topic
     */
    class PositionTracker
    {
    public:
        friend class ::PositionTrackerTest;

        /**
         * @brief Construct a new Position Tracker object
         *
         * @details This constructor is used to construct a new PositionTracker object and start the thread
         * 
         * @param _pub_ptr shared pointer to publisher
         * @param _positioner unique pointer to positioner
         */
        PositionTracker(std::shared_ptr<simplepubsub::IPublisher> _pub_ptr, std::unique_ptr<IPositioner> _positioner)
            : keepRunning(true),
              pub_ptr(_pub_ptr),
              positioner(std::move(_positioner))
        {
            positionerThread = std::thread(&PositionTracker::getData, this);
        };

        /**
         * @brief Destroy the Position Tracker object
         */
        ~PositionTracker()
        {
            keepRunning = false;
            positionerThread.join();
        };

    private:
        /**
         * @brief Get the Data object
         */
        void getData();

        /**
         * @brief Pack the Status object
         *
         * @return const std::string formatted status
         */
        const std::string packStatus();

        /**
         * @brief Pack the Position object
         *
         * @return const std::string formatted position
         */
        const std::string packPosition();

        /**
         * @brief thread keep running flag
         */
        std::atomic<bool> keepRunning;

        /**
         * @brief shared pointer to publisher
         */
        std::shared_ptr<simplepubsub::IPublisher> pub_ptr;

        /**
         * @brief unique pointer to positioner
         */
        std::unique_ptr<IPositioner> positioner;

        /**
         * @brief thread for positioner
         */
        std::thread positionerThread;

        /**
         * @struct position_xyz
         * 
         * @brief position struct
         */
        struct position_xyz
        {
            int x;
            int y;
            int z;
        };

        /**
         * @brief position struct
         */
        struct position_xyz position;

        /**
         * @brief positioner status
         */
        int status;
    };
}

#endif /* __POSITIONTRACKER_H */