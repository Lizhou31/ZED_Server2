/**
 * @file simplepubsub_test.h
 * @author Lizhou (lisie31s@gmail.com)
 * @brief simplepubsub test header
 * @version 0.1
 * @date 2024-01-15
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <gtest/gtest.h>
#include <SimplePubSub.hpp>

/**
 * @brief Construct a new test test_publish_subscribe object
 */
class SimplePubSubTest : public ::testing::Test
{
protected:
    /**
     * @brief Construct a new Simple Pub Sub Test object
     */
    SimplePubSubTest():callback_return(false) {}

    /**
     * @brief Destroy the Simple Pub Sub Test object
     */
    ~SimplePubSubTest() override {}

    /**
     * @brief Set up the Simple Pub Sub Test object
     */
    void SetUp() override
    {
        callback_return = false;
    }

    /**
     * @brief Tear down the Simple Pub Sub Test object
     */
    void TearDown() override
    {
    }

    /**
     * @brief subscription callback
     * 
     * @param topic 
     * @param data 
     */
    void subscription_callback(const std::string &topic, const std::string &data)
    {
        EXPECT_EQ("TestTopic", topic);
        EXPECT_EQ("test", data);
        callback_return = true;
    }

    /**
     * @brief callback return when subscription callback is called
     * 
     */
    std::atomic<bool> callback_return;
};