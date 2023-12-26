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
#include <iostream>
#include <string>

namespace simplepubsub
{
    class publish_worker{
        
    };
    class Publisher
    {
    public:
        Publisher()
        {
            socket.bind("inproc://my_endpoint");
        }

        void publish(const std::string &data)
        {
            zmq::message_t message(data.begin(), data.end());
            socket.send(message, zmq::send_flags::none);
        }

    private:
        zmq::context_t context;
        zmq::socket_t socket{context, zmq::socket_type::pub};
    };

    class Subscriber
    {
    public:
        Subscriber(zmq::context_t &context) : socket(context, zmq::socket_type::sub)
        {
            socket.connect("inproc://my_endpoint");
            socket.set(zmq::sockopt::subscribe, "");
        }

        void listen()
        {
            while (true)
            {
                zmq::message_t message;
                socket.recv(message, zmq::recv_flags::none);
                std::string data(static_cast<char *>(message.data()), message.size());
                // Process data
                std::cout << "Received: " << data << std::endl;
            }
        }

    private:
        zmq::socket_t socket;
    };

}

#endif /* __SIMPLEPUBSUB_HPP */