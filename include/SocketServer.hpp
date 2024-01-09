#ifndef SOCKETSERVER_H
#define SOCKETSERVER_H

#include <fstream>
#include "Socket.hpp"
#include "CommandSolver.hpp"
class SocketServerTest;
namespace mysocketserver
{
    class SocketServer
    {
    public:
        friend class ::SocketServerTest;
        SocketServer(std::unique_ptr<::SocketFactory> _factory,
                     std::shared_ptr<simplepubsub::IPublisher> invoker_pub) : factory(std::move(_factory)),
                                                                              socket(nullptr),
                                                                              invoker(invoker_pub) {}
        void init(int port);
        void waiting_connection(); // blocking
        void waiting_command();    // blocking
        void execute_command();    // non-blocking

        /* Subscriber Method */
        void createFile_callback(const std::string &topic, const std::string &data);
        void stop_callback(const std::string &topic, const std::string &data);
        void probe_callback(const std::string &topic, const std::string &data);
        void getInfo_callback(const std::string &topic, const std::string &data);

        void register_subscriber(simplepubsub::IAgent &agent); // TODO: might refctor.

        // TODO: ZED_Status callback, ZED_Position callback

        void shutdown();
        ~SocketServer()
        {
            shutdown();
        };

    private:
        std::unique_ptr<::SocketFactory> factory;
        Socket *socket;
        std::string message;
        commandsolver::CommandInvoker invoker;
        std::ofstream result_file;
        std::unique_ptr<simplepubsub::ISubscriber> topic_create, topic_stop, topic_probe, topic_getInfo;
    };
}

#endif // SOCKETSERVER_H