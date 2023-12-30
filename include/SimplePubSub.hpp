/**
 * @file SimplePubSub.hpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-12-25
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef __SIMPLEPUBSUB_HPP
#define __SIMPLEPUBSUB_HPP

#include <zmq.hpp>
#include <zmq_addon.hpp>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <iostream>
namespace simplepubsub
{
    class IPublisher
    {
        virtual void publish(const std::string &topic, const std::string &data) = 0;
    };
    
    class Publisher: public IPublisher
    {
    public:
        Publisher(zmq::context_t *ctx) : socket(*ctx, ZMQ_PUB), keepRunning(true)
        {
            socket.bind("inproc://zedserver");
            senderThread = std::thread(&Publisher::processMessageQueue, this);
        }

        ~Publisher()
        {
            keepRunning = false;
            messageQueueCond.notify_all();
            senderThread.join();
            socket.close();
        }

        void publish(const std::string &topic, const std::string &data) override
        {
            std::lock_guard<std::mutex> lock(queueMutex);
            messageQueue.emplace(topic, data);
            messageQueueCond.notify_one();
        }

    private:
        zmq::socket_t socket;
        std::thread senderThread;
        std::queue<std::pair<std::string, std::string>> messageQueue;
        std::mutex queueMutex;
        std::condition_variable messageQueueCond;
        std::atomic<bool> keepRunning;

        void processMessageQueue()
        {
            while (keepRunning)
            {
                std::unique_lock<std::mutex> lock(queueMutex);
                messageQueueCond.wait(lock, [this]
                                      { return !messageQueue.empty() || !keepRunning; });

                while (!messageQueue.empty())
                {
                    std::string topic, data;
                    std::tie(topic, data) = messageQueue.front();
                    messageQueue.pop();
                    lock.unlock();
                    zmq::message_t topicMsg(topic.begin(), topic.end());
                    zmq::message_t dataMsg(data.begin(), data.end());
                    socket.send(topicMsg, zmq::send_flags::sndmore | zmq::send_flags::dontwait);
                    socket.send(dataMsg, zmq::send_flags::dontwait);

                    lock.lock();
                }
            }
        }
    };

    class Subscriber
    {
    public:
        Subscriber(zmq::context_t *ctx, const std::string &topic) : socket(*ctx, ZMQ_SUB), keepRunning(true)
        {
            socket.connect("inproc://zedserver");
            socket.set(zmq::sockopt::subscribe, topic); // Subscribe to all topics
            receiverThread = std::thread(&Subscriber::receiveMessages, this);
        }

        ~Subscriber()
        {
            keepRunning = false;
            receiverThread.join();
            socket.close();
        }

        void onMessageReceived(const std::function<void(const std::string &, const std::string &)> &callback)
        {
            messageReceivedCallback = callback;
        }

    private:
        zmq::socket_t socket;
        std::thread receiverThread;
        std::atomic<bool> keepRunning;
        std::function<void(const std::string &, const std::string &)> messageReceivedCallback;

        void receiveMessages()
        {
            zmq::pollitem_t items[] = {{static_cast<void *>(socket), 0, ZMQ_POLLIN, 0}};
            while (keepRunning)
            {
                zmq::poll(&items[0], 1, std::chrono::milliseconds(10)); // Non-blocking poll

                if (items[0].revents & ZMQ_POLLIN)
                {
                    zmq::message_t topicMsg;
                    zmq::message_t dataMsg;
                    socket.recv(topicMsg, zmq::recv_flags::dontwait);
                    socket.recv(dataMsg, zmq::recv_flags::dontwait);

                    std::string topic(static_cast<char *>(topicMsg.data()), topicMsg.size());
                    std::string data(static_cast<char *>(dataMsg.data()), dataMsg.size());
                    if (messageReceivedCallback)
                    {
                        messageReceivedCallback(topic, data);
                    }
                }
            }
        }
    };
    class Agent
    {
    public:
        Agent(){};

        std::unique_ptr<Publisher> requestPublisher()
        {
            return std::make_unique<Publisher>(&ctx);
        }

        std::unique_ptr<Subscriber> requestSubcriber(const std::string &topic, const std::function<void(const std::string &, const std::string &)> &callback)
        {
            auto result = std::make_unique<Subscriber>(&ctx, "TestTopic");
            result->onMessageReceived(callback);
            return result;
        }

    private:
        zmq::context_t ctx;
    };
}

#endif /* __SIMPLEPUBSUB_HPP */