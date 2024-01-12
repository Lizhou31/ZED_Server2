/**
 * @file PositionTracker.hpp
 * @author your name (you@domain.com)
 * @brief
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

namespace positiontracker
{
    enum Positioner_Status {OK, Searching, Save, Stop};
    class IPositioner
    {
    public:
        virtual ~IPositioner(){};
        virtual void getStatus(int &) = 0;
        virtual void getPosition(int &, int &, int &) = 0;
    };

    class PositionTracker
    {
    public:
        friend class ::PositionTrackerTest;
        PositionTracker(simplepubsub::IAgent &agent, std::unique_ptr<IPositioner> _positioner)
            : keepRunning(true),
              positioner(std::move(_positioner))
        {
            pub_ptr = agent.requestPublisher();
            positionerThread = std::thread(&PositionTracker::getData, this);
        };
        ~PositionTracker()
        {
            keepRunning = false;
            positionerThread.join();
        };

    private:
        void getData();
        const std::string packStatus();
        const std::string packPosition();
        std::atomic<bool> keepRunning;
        std::unique_ptr<simplepubsub::IPublisher> pub_ptr;
        std::unique_ptr<IPositioner> positioner;
        std::thread positionerThread;
        struct position_xyz
        {
            int x;
            int y;
            int z;
        };
        struct position_xyz position;
        int status;
    };
}

#endif /* __POSITIONTRACKER_H */