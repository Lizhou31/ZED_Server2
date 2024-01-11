/**
 * @file positiontracker_test.h
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-01-10
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef __POSITIONTRACKER_TEST_H
#define __POSITIONTRACKER_TEST_H

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <PositionTracker.hpp>

class MockPositioner : public positiontracker::IPositioner
{
public:
    MOCK_METHOD(void, getStatus, (int &), (override));
    MOCK_METHOD(void, getPosition, (int &, int &, int &), (override));
};

class MockPublish : public simplepubsub::IPublisher
{
public:
    MOCK_METHOD(void, publish, (const std::string &topic, const std::string &data), (override));
};

class MockAgent : public simplepubsub::IAgent
{
public:
    MOCK_METHOD(std::unique_ptr<simplepubsub::IPublisher>, requestPublisher, (), (override));
    MOCK_METHOD(std::unique_ptr<simplepubsub::ISubscriber>, requestSubcriber,
                (const std::string &topic, const std::function<void(const std::string &, const std::string &)> &callback), (override));
};

class PositionTrackerTest : public ::testing::Test
{
protected:
    PositionTrackerTest() {}
    ~PositionTrackerTest() override {}
    MockPublish *pub_ptr;
    std::unique_ptr<positiontracker::PositionTracker> positionTracker;
    MockAgent agent;
    MockPositioner *positioner;
    std::atomic<int> status_ok, position_ok;
    void SetUp() override
    {
        pub_ptr = new MockPublish();
        positioner = new MockPositioner();
        status_ok = 0;
        position_ok = 0;
    }

    void TearDown() override
    {
        positionTracker.reset();
    }

    int get_status(){
        return positionTracker->status;
    }

    int get_positionX(){
        return positionTracker->position.x;
    }
    int get_positionY(){
        return positionTracker->position.y;
    }
    int get_positionZ(){
        return positionTracker->position.z;
    }
};

#endif /* __POSITIONTRACKER_TEST_H */