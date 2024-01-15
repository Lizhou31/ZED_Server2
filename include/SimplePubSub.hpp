/**
 * @file SimplePubSub.hpp
 * @author Lizhou (lisie31s@gmail.com)
 * @brief SimplePubSub header file
 * @details This is a sipmle pub-sub library based on ZeroMQ
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

namespace simplepubsub
{
    /**
     * @class IPublisher
     *
     * @brief Publisher interface
     */
    class IPublisher
    {
    public:
        /**
         * @brief Destroy the IPublisher object
         */
        virtual ~IPublisher() {}

        /**
         * @brief Publish data to a topic
         *
         * @param topic topic name
         * @param data data to be published
         */
        virtual void publish(const std::string &topic, const std::string &data) = 0;
    };

    /**
     * @class Publisher
     *
     * @brief Publisher class
     *
     * @details This class is used to publish data to a topic, only support one publisher per application.
     *
     * @todo Support multiple publishers per application.
     */
    class Publisher : public IPublisher
    {
    public:
        /**
         * @brief Construct a new Publisher object
         *
         * @param _ctx ZeroMQ context
         */
        Publisher(zmq::context_t *_ctx) : ctx(_ctx), keepRunning(true), socketInit(false)
        {
            senderThread = std::thread(&Publisher::processMessageQueue, this);
        }

        /**
         * @brief Destroy the Publisher object
         */
        ~Publisher()
        {
            keepRunning = false;
            messageQueueCond.notify_all();
            senderThread.join();
        }

        /**
         * @brief Publish data to a topic
         *
         * @details This function is used to publish data to a topic. This is non-blocking function,
         *          use queue to store data and send it in another thread.
         *
         * @param topic topic name
         * @param data data to be published
         */
        void publish(const std::string &topic, const std::string &data) override
        {
            while (!socketInit)
                ;
            std::lock_guard<std::mutex> lock(queueMutex);
            messageQueue.emplace(topic, data);
            messageQueueCond.notify_one();
        }

    private:
        /**
         * @brief Process message queue
         *
         * @details This thread function is used to process message queue, send data to subscribers.
         *          From ZeroMQ documentation, socekt is not thread-safe, so we need create a socket per thread.
         */
        void processMessageQueue()
        {
            socket = std::make_unique<zmq::socket_t>(*ctx, ZMQ_PUB);
            socket->bind("inproc://zedserver");
            socketInit = true;

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
                    socket->send(topicMsg, zmq::send_flags::sndmore | zmq::send_flags::dontwait);
                    socket->send(dataMsg, zmq::send_flags::dontwait);

                    lock.lock();
                }
            }
            socket->close();
        }

        /**
         * @brief ZeroMQ socket
         */
        std::unique_ptr<zmq::socket_t> socket;

        /**
         * @brief ZeroMQ context
         */
        zmq::context_t *ctx;

        /**
         * @brief Sender thread
         */
        std::thread senderThread;

        /**
         * @brief Message queue
         */
        std::queue<std::pair<std::string, std::string>> messageQueue;

        /**
         * @brief Message queue mutex
         */
        std::mutex queueMutex;

        /**
         * @brief Message queue condition variable
         */
        std::condition_variable messageQueueCond;

        /**
         * @brief Keep running flag
         */
        std::atomic<bool> keepRunning;

        /**
         * @brief Socket init flag
         */
        std::atomic<bool> socketInit;
    };

    class ISubscriber
    {
    public:
        /**
         * @brief Destroy the ISubscriber object
         */
        virtual ~ISubscriber() {}

        /**
         * @brief Set callback function when message received
         *
         * @param callback callback function
         */
        virtual void onMessageReceived(const std::function<void(const std::string &, const std::string &)> &callback) = 0;
    };

    /**
     * @class Subscriber
     *
     * @brief Subscriber class
     *
     * @details This class is used to subscribe a topic.
     */
    class Subscriber : public ISubscriber
    {
    public:
        /**
         * @brief Construct a new Subscriber object
         *
         * @param _ctx ZeroMQ context
         * @param topic topic name
         */
        Subscriber(zmq::context_t *_ctx, const std::string &topic) : ctx(_ctx), topic(topic), keepRunning(true), socketInit(false)
        {
            receiverThread = std::thread(&Subscriber::receiveMessages, this);
        }

        /**
         * @brief Destroy the Subscriber object
         */
        ~Subscriber()
        {
            keepRunning = false;
            receiverThread.join();
        }

        /**
         * @brief Set callback function when message received
         *
         * @param callback callback function
         */
        void onMessageReceived(const std::function<void(const std::string &, const std::string &)> &callback) override
        {
            while (!socketInit)
                ;
            messageReceivedCallback = callback;
        }

    private:

        /**
         * @brief Receive messages
         *
         * @details This thread function is used to receive messages from publisher.
         *          From ZeroMQ documentation, socekt is not thread-safe, so we need create a socket per thread.
         */
        void receiveMessages()
        {
            socket = std::make_unique<zmq::socket_t>(*ctx, ZMQ_SUB);
            socket->connect("inproc://zedserver");
            socket->set(zmq::sockopt::subscribe, topic);
            socketInit = true;

            zmq::pollitem_t items[] = {{static_cast<void *>(*socket), 0, ZMQ_POLLIN, 0}};
            while (keepRunning)
            {
                zmq::poll(&items[0], 1, std::chrono::milliseconds(10)); // Non-blocking poll
                if (items[0].revents & ZMQ_POLLIN)
                {
                    zmq::message_t topicMsg;
                    zmq::message_t dataMsg;
                    socket->recv(topicMsg, zmq::recv_flags::dontwait);
                    socket->recv(dataMsg, zmq::recv_flags::dontwait);

                    std::string topic(static_cast<char *>(topicMsg.data()), topicMsg.size());
                    std::string data(static_cast<char *>(dataMsg.data()), dataMsg.size());
                    if (messageReceivedCallback)
                    {
                        messageReceivedCallback(topic, data);
                    }
                }
            }
            socket->close();
        }

        /**
         * @brief ZeroMQ socket
         */
        std::unique_ptr<zmq::socket_t> socket;

        /**
         * @brief ZeroMQ context
         */
        zmq::context_t *ctx;

        /**
         * @brief Topic name
         */
        const std::string &topic;

        /**
         * @brief Receiver thread
         */
        std::thread receiverThread;

        /**
         * @brief Message received callback function
         */
        std::atomic<bool> keepRunning;

        /**
         * @brief Socket init flag
         */
        std::atomic<bool> socketInit;

        /**
         * @brief Message received callback function
         */
        std::function<void(const std::string &, const std::string &)> messageReceivedCallback;
    };

    /**
     * @class IAgent
     *
     * @brief Agent interface
     */
    class IAgent
    {
    public:
        /**
         * @brief Destroy the IAgent object
         */
        virtual ~IAgent(){};

        /**
         * @brief Request publisher
         *
         * @return std::unique_ptr<IPublisher> publisher pointer
         */
        virtual std::unique_ptr<IPublisher> requestPublisher() = 0;

        /**
         * @brief Request subscriber
         *
         * @param topic topic name
         * @param callback callback function
         * @return std::unique_ptr<ISubscriber> subscriber pointer
         */
        virtual std::unique_ptr<ISubscriber> requestSubcriber(const std::string &topic,
                                                              const std::function<void(const std::string &, const std::string &)> &callback) = 0;
    };

    /**
     * @class Agent
     *
     * @brief Agent class
     *
     * @details This class is used to request publisher and subscriber.
     */
    class Agent : public IAgent
    {
    public:
        /**
         * @brief Construct a new Agent object
         */
        Agent(){};

        /**
         * @brief Request publisher
         * 
         * @return std::unique_ptr<IPublisher> 
         */
        std::unique_ptr<IPublisher> requestPublisher() override
        {
            return std::make_unique<Publisher>(&ctx);
        }

        /**
         * @brief Request subscriber
         *
         * @param topic topic name
         * @param callback callback function
         * @return std::unique_ptr<ISubscriber> subscriber pointer
         */
        std::unique_ptr<ISubscriber> requestSubcriber(const std::string &topic,
                                                      const std::function<void(const std::string &, const std::string &)> &callback) override
        {
            auto result = std::make_unique<Subscriber>(&ctx, topic);
            result->onMessageReceived(callback);
            return result;
        }

    private:

        /**
         * @brief ZeroMQ context
         * 
         * @details This context is shared by all publishers and subscribers. From ZeroMQ documentation, 
         *          inproc communication is based on shared memory in the same context.
         */
        zmq::context_t ctx;
    };
}

#endif /* __SIMPLEPUBSUB_HPP */