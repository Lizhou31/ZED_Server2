/**
 * @file positiontracker_test.h
 * @author Lizhou (lisie31s@gmail.com)
 * @brief positiontracker test header
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

/**
 * @brief MockPositioner class for testing
 */
class MockPositioner : public positiontracker::IPositioner
{
public:
    MOCK_METHOD(void, getStatus, (int &), (override));
    MOCK_METHOD(void, getPosition, (int &, int &, int &), (override));
};

/**
 * @brief MockPublish class for testing
 */
class MockPublish : public simplepubsub::IPublisher
{
public:
    MOCK_METHOD(void, publish, (const std::string &topic, const std::string &data), (override));
};

/**
 * @brief PositionTrackerTest class for testing
 */
class PositionTrackerTest : public ::testing::Test
{
protected:
    /**
     * @brief Construct a new Position Tracker Test object
     */
    PositionTrackerTest() {}

    /**
     * @brief Destroy the Position Tracker Test object
     */
    ~PositionTrackerTest() override {}

    /**
     * @brief Set up the Position Tracker Test object
     */
    void SetUp() override
    {
        pub_ptr = new MockPublish();
        positioner = new MockPositioner();
        status_ok = 0;
        position_ok = 0;
    }

    /**
     * @brief Tear down the Position Tracker Test object
     */
    void TearDown() override
    {
        positionTracker.reset();
    }

    /**
     * @brief Get the status object
     * 
     * @return int status
     */
    int get_status()
    {
        return positionTracker->status;
    }

    /**
     * @brief Get the positionX object
     * 
     * @return int positionX
     */
    int get_positionX()
    {
        return positionTracker->position.x;
    }

    /**
     * @brief Get the positionY object
     * 
     * @return int positionY
     */
    int get_positionY()
    {
        return positionTracker->position.y;
    }

    /**
     * @brief Get the positionZ object
     * 
     * @return int positionZ
     */
    int get_positionZ()
    {
        return positionTracker->position.z;
    }

    /**
     * @brief MockPublish smart pointer
     */
    MockPublish *pub_ptr;

    /**
     * @brief MockPositioner smart pointer
     */
    std::unique_ptr<positiontracker::PositionTracker> positionTracker;

    /**
     * @brief MockPositioner raw pointer
     */
    MockPositioner *positioner;

    /**
     * @brief status_ok atomic variable
     */
    std::atomic<int> status_ok;

    /**
     * @brief position_ok atomic variable
     */
    std::atomic<int> position_ok;
};

#endif /* __POSITIONTRACKER_TEST_H */