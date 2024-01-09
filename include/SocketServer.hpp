#ifndef SOCKETSERVER_H
#define SOCKETSERVER_H

#include <fstream>
#include "Socket.hpp"
#include <atomic>
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
                                                                              invoker(invoker_pub),
                                                                              zed_status(0) {}
        void init(int port);
        void waiting_connection();        // blocking
        void waiting_command();           // blocking
        void sending_data(std::string &); // blocking
        void execute_command();           // non-blocking

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
        std::atomic<int> zed_status;

        // Packed the return data
        // TODO: Refactor to independent file
        std::string pack_infoData()
        {
            const std::string status_list[] = {"OK","Searching", "Saving", "Stop"};
            nlohmann::json info;
            info["Command"] = 6;
            info["Status"] = status_list[zed_status.load()];
            return nlohmann::to_string(info);
        }
    };
}

#endif // SOCKETSERVER_H