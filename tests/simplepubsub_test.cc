#include "simplepubsub_test.h"

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
    while(callback_return);
}