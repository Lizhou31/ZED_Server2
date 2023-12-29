#include <gtest/gtest.h>
#include <SimplePubSub.hpp>

class SimplePubSubTest : public ::testing::Test
{
protected:
    SimplePubSubTest():callback_return(false) {}
    ~SimplePubSubTest() override {}

    void SetUp() override
    {
        callback_return = false;
    }

    void TearDown() override
    {
    }

    void subscription_callback(const std::string &topic, const std::string &data)
    {
        EXPECT_EQ("TestTopic", topic);
        EXPECT_EQ("test", data);
        callback_return = true;
    }
    std::atomic<bool> callback_return;
};