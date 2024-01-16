/**
 * @file SocketServer.hpp
 * @author Lizhou (lisie31s@gmail.com)
 * @brief Socket server header
 * @version 0.1
 * @date 2024-01-15
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef SOCKETSERVER_H
#define SOCKETSERVER_H

#include <fstream>
#include <atomic>
#include "Socket.hpp"
#include "CommandSolver.hpp"
#include "PositionTracker.hpp"

/**
 * @brief Use in deployment environment
 */
#ifdef USE_DEPLOYMENT_ENV
#include "ZedPositioner.hpp"
#else
/**
 * @brief This definition origin defined in ZedPositioner.hpp
 */
#define ZED_FACTOR 10000
#endif

class SocketServerTest;

namespace mysocketserver
{
    /**
     * @class SocketServer
     *
     * @brief Socket server class
     */
    class SocketServer
    {
    public:
        friend class ::SocketServerTest;

        /**
         * @brief Construct a new Socket Server object
         *
         * @param _factory This is a factory for creating socket (WebSocket or UnixSocket)
         * @param invoker_pub This is a publisher for publish the result of command and status of ZED
         *
         * @note The invoker_pub is a shared_ptr, it shared with the invoker and positioner_pub.
         *
         * @todo Refactor the invoker_pub and positioner_pub to independent object.
         */
        SocketServer(std::unique_ptr<::SocketFactory> _factory,
                     std::shared_ptr<simplepubsub::IPublisher> invoker_pub) : factory(std::move(_factory)),
                                                                              socket(nullptr),
                                                                              invoker(invoker_pub),
                                                                              positioner_pub(invoker_pub),
                                                                              zed_status(positiontracker::Positioner_Status::Stop),
                                                                              zed_x(0),
                                                                              zed_y(0),
                                                                              zed_z(0) {}
        /**
         * @brief Initialize the socket server
         *
         * @param port The port of socket server
         */
        void init(int port);

        /**
         * @brief Waiting for connection
         *
         * @note This method is a blocking method.
         */
        void waiting_connection();

        /**
         * @brief Waiting for command
         *
         * @note This method is a blocking method.
         */
        void waiting_command();

        /**
         * @brief Execute the command
         *
         * @note This method is a non-blocking method.
         */
        void execute_command();

        /**
         * @brief Shutdown the socket server
         */
        void shutdown();

        /* Subscriber callback functions */
        /**
         * @brief CREATE command callback function
         *
         * @param topic The topic of subscription
         * @param data The data of subscription
         */
        void createFile_callback(const std::string &topic, const std::string &data);

        /**
         * @brief STOP command callback function
         *
         * @param topic The topic of subscription
         * @param data The data of subscription
         */
        void stop_callback(const std::string &topic, const std::string &data);

        /**
         * @brief PROBE command callback function
         *
         * @param topic The topic of subscription
         * @param data The data of subscription
         */
        void probe_callback(const std::string &topic, const std::string &data);

        /**
         * @brief GET_INFO command callback function
         *
         * @param topic The topic of subscription
         * @param data The data of subscription
         */
        void getInfo_callback(const std::string &topic, const std::string &data);

        /**
         * @brief ZED_Status callback function
         *
         * @param topic The topic of subscription
         * @param data The data of subscription
         */
        void zedStatus_callback(const std::string &topic, const std::string &data);

        /**
         * @brief ZED_Position callback function
         *
         * @param topic The topic of subscription
         * @param data The data of subscription
         */
        void zedPosition_callback(const std::string &topic, const std::string &data);

        /**
         * @brief Register the subscriber
         *
         * @param agent The agent of simplepubsub
         *
         * @todo Refacor
         */
        void register_subscriber(simplepubsub::IAgent &agent); // TODO: might refctor.

        /**
         * @brief Destroy the Socket Server object
         */
        ~SocketServer()
        {
            shutdown();
        };

    private:

        /**
         * @brief Pack the info data
         *
         * @return std::string The info data
         * 
         * @todo Refactor to independent file and return const?
         */
        std::string pack_infoData()
        {
            // TODO: Refactor to independent file and return const?
            const std::string status_list[] = {"OK", "Searching", "Saving", "Stop"};
            nlohmann::json info;
            info["Command"] = 6;
            info["Status"] = status_list[zed_status.load()];
            info["X"] = float(zed_x.load()) / ZED_FACTOR;
            info["Y"] = float(zed_y.load()) / ZED_FACTOR;
            info["Z"] = float(zed_z.load()) / ZED_FACTOR;
            return nlohmann::to_string(info);
        }

        /**
         * @brief Set the ZED position to zero
         */
        void zed_setZero()
        {
            // TODO: Add Test for this method.
            zed_status.store(positiontracker::Positioner_Status::Stop);
            zed_x.store(0);
            zed_y.store(0);
            zed_z.store(0);
        }

        /**
         * @brief The factory for creating socket (WebSocket or UnixSocket)
         */
        std::unique_ptr<::SocketFactory> factory;

        /**
         * @brief The socket
         */
        Socket *socket;

        /**
         * @brief The message of socket
         */
        std::string message;

        /**
         * @brief The command invoker
         */
        commandsolver::CommandInvoker invoker;

        /**
         * @brief The result file
         */
        std::ofstream result_file;

        /**
         * @brief The subscriber of topics
         */
        std::unique_ptr<simplepubsub::ISubscriber> topic_create, topic_stop, topic_probe, topic_getInfo, topic_status, topic_position;

        /**
         * @brief The position tracker
         */
        std::unique_ptr<positiontracker::PositionTracker> pt;

        /**
         * @brief The publisher for publish the result of command and status of ZED
         */
        std::shared_ptr<simplepubsub::IPublisher> positioner_pub;

        /**
         * @brief The status of ZED
         */
        std::atomic<int> zed_status;

        /**
         * @brief The position of ZED
         */
        std::atomic<int> zed_x, zed_y, zed_z;
    };
}

#endif // SOCKETSERVER_H