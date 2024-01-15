/**
 * @file simplepubsub_test.cc
 * @author Lizhou (lisie31s@gmail.com)
 * @brief simplepubsub test
 * @version 0.1
 * @date 2024-01-15
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "simplepubsub_test.h"

/**
 * @brief Construct a new test test_publish_subscribe object
 */
TEST_F(SimplePubSubTest, test_publish_subscribe)
{
    zmq::context_t ctx;
    simplepubsub::Subscriber sub(&ctx, "TestTopic");
    std::function<void(const std::string &topic, const std::string &data)> subscription_callback = [this](const std::string &topic, const std::string &data)
    { this->subscription_callback(topic, data); };
    sub.onMessageReceived(subscription_callback);

    simplepubsub::Publisher pub(&ctx);
    pub.publish("TestTopic", "test");
    pub.publish("FakeTopic", "faker");
    while (!callback_return)
        ;
}

/**
 * @brief Construct a new test test_agent_publish_subcribe object
 */
TEST_F(SimplePubSubTest, test_agent_publish_subcribe)
{
    simplepubsub::Agent agent;

    std::unique_ptr<simplepubsub::IPublisher> publisher_ptr;
    publisher_ptr = agent.requestPublisher();

    std::unique_ptr<simplepubsub::ISubscriber> subcriber_ptr;
    std::function<void(const std::string &topic,
                       const std::string &data)>
        subscription_callback = [this](const std::string &topic, const std::string &data)
    { this->subscription_callback(topic, data); };
    subcriber_ptr = agent.requestSubcriber("TestTopic", subscription_callback);

    publisher_ptr->publish("TestTopic", "test");
    publisher_ptr->publish("FakeTopic", "faker");

    while (!callback_return)
        ;
}