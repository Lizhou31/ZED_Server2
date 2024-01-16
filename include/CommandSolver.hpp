/**
 * @file CommandSolver.hpp
 * @author Lizhou31 (lisie31s@gmail.com)
 * @brief  CommandSolver class declaration
 * @version 0.1
 * @date 2024-01-15
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef COMMANDSOLVER_H
#define COMMANDSOLVER_H

#include "SimplePubSub.hpp"
#include <nlohmann/json.hpp>
#include <iostream>
#include <string>

class CommandSolverTest;

namespace commandsolver
{
    /**
     * @class ICommand
     *
     * @brief Interface for Command
     */
    class ICommand
    {
    public:
        /**
         * @brief Destroy the ICommand object
         */
        virtual ~ICommand() {}

        /**
         * @brief Execute the command
         *
         * @param ptr publisher smart pointer
         */
        virtual void execute(std::shared_ptr<simplepubsub::IPublisher> ptr) = 0;
    };

    /**
     * @class CreateCommand
     *
     * @brief Create command class declaration
     */
    class CreateCommand : public ICommand
    {
        friend class ::CommandSolverTest;

    public:
        /**
         * @brief Construct a new CreateCommand object
         *
         * @param _id       file name
         * @param _radius   file radius
         * @param _height   file height
         */
        CreateCommand(std::string _id, std::string _radius, std::string _height)
        {
            data.id = _id;
            data.radius = _radius;
            data.height = _height;
        };

        /**
         * @brief Destroy the CreateCommand object
         */
        void execute(std::shared_ptr<simplepubsub::IPublisher> ptr) override;

    private:
        /**
         * @struct createFileData
         *
         * @brief Create file data structure
         */
        struct createFileData
        {
            std::string id;
            std::string radius;
            std::string height;
        };

        /**
         * @brief Create file data
         */
        struct createFileData data;
    };

    /**
     * @class ProbeCommand
     *
     * @brief Probe command class declaration
     */
    class ProbeCommand : public ICommand
    {
    public:
        /**
         * @brief Construct a new ProbeCommand object
         *
         * @param _sensor_data sensor data from probe
         */
        ProbeCommand(std::string _sensor_data) : sensor_data(_sensor_data){};

        /**
         * @brief Destroy the ProbeCommand object
         */
        void execute(std::shared_ptr<simplepubsub::IPublisher> ptr) override;

    private:
        /**
         * @brief sensor data from probe
         */
        std::string sensor_data;
    };

    /**
     * @class StopCommand
     *
     * @brief Stop command class declaration
     */
    class StopCommand : public ICommand
    {
    public:
        /**
         * @brief Construct a new StopCommand object
         */
        StopCommand(){};

        /**
         * @brief Destroy the StopCommand object
         */
        void execute(std::shared_ptr<simplepubsub::IPublisher> ptr) override;
    };

    /**
     * @class GetInfoCommand
     *
     * @brief GetInfo command class declaration
     */
    class GetInfoCommand : public ICommand
    {
    public:
        /**
         * @brief Construct a new GetInfoCommand object
         */
        GetInfoCommand(){};

        /**
         * @brief Destroy the GetInfoCommand object
         */
        void execute(std::shared_ptr<simplepubsub::IPublisher> ptr) override;
    };

    /**
     * @class CommandFactory
     *
     * @brief Command factory class declaration
     *
     * @details This class is used to create the command
     *
     * @todo Add more command type: RESET, CHANGE
     */
    class CommandFactory
    {
    public:
        /**
         * @brief Create a Command object
         *
         * @param commandJson command json
         * @return std::unique_ptr<ICommand>
         * 
         * @todo Refactor decode method
         */
        static std::unique_ptr<ICommand> createCommand(const nlohmann::json &commandJson)
        {
            auto commandType = commandJson["Command"].get<int>();
            if (commandType == 0) // CREATE
            {
                auto args = commandJson["Args"];
                try
                {
                    // TODO: Refactor this type decode
                    return std::make_unique<CreateCommand>((args[0])["ID"].get<std::string>(),
                                                           (args[1])["Radius"].get<std::string>(),
                                                           (args[2])["Height"].get<std::string>());
                }
                catch (std::exception &e)
                {
                    std::cerr << e.what() << std::endl;
                    throw;
                }
            }
            else if (commandType == 1) // PROBE
            {
                auto args = commandJson["Args"];
                try
                {
                    return std::make_unique<ProbeCommand>(args[0]);
                }
                catch (const std::exception &e)
                {
                    std::cerr << e.what() << std::endl;
                    throw;
                }
            }
            else if (commandType == 4) // STOP
            {
                try
                {
                    return std::make_unique<StopCommand>();
                }
                catch (const std::exception &e)
                {
                    std::cerr << e.what() << std::endl;
                    throw;
                }
            }
            else if (commandType == 5) // GETINFO
            {
                try
                {
                    return std::make_unique<GetInfoCommand>();
                }
                catch (const std::exception &e)
                {
                    std::cerr << e.what() << std::endl;
                    throw;
                }
            }
            else
                return nullptr;
            // TODO: RESET = 2,
            // TODO: CHANGE = 3
        }
    };

    /**
     * @class CommandInvoker
     *
     * @brief Command invoker class declaration
     *
     * @details This class is used to execute the command
     */
    class CommandInvoker
    {
    public:
        /**
         * @brief Construct a new Command Invoker object
         *
         * @param ptr publisher smart pointer
         */
        CommandInvoker(std::shared_ptr<simplepubsub::IPublisher> ptr) : _publisher_ptr(ptr) {}

        /**
         * @brief Execute the command
         *
         * @param rawCommand raw command string
         */
        void executeCommand(const std::string &rawCommand)
        {
            std::unique_ptr<ICommand> command;
            try
            {
                auto commandJson = nlohmann::json::parse(rawCommand);
                command = CommandFactory::createCommand(commandJson);
            }
            catch (std::exception &e)
            {
                std::cerr << e.what() << std::endl;
                throw std::invalid_argument("Input command error");
            }
            command->execute(_publisher_ptr);
        }

        /**
         * @brief Destroy the Command Invoker object
         */
        ~CommandInvoker()
        {
            _publisher_ptr.reset();
        }

    private:
        /**
         * @brief publisher smart pointer
         */
        std::shared_ptr<simplepubsub::IPublisher> _publisher_ptr;
    };

}
#endif /* COMMANDSOLVER_H */
