#ifndef COMMANDSOLVER_H
#define COMMANDSOLVER_H

#include "SimplePubSub.hpp"
#include <nlohmann/json.hpp>
#include <iostream>
#include <string>

class CommandSolverTest;

namespace commandsolver
{
    class ICommand
    {
    public:
        virtual ~ICommand() {}
        virtual void execute(std::shared_ptr<simplepubsub::IPublisher> ptr) = 0;
    };

    class CreateCommand : public ICommand
    {
        friend class ::CommandSolverTest;

    public:
        CreateCommand(std::string _id, std::string _redius, std::string _height)
        {
            data.id = _id;
            data.redius = _redius;
            data.height = _height;
        };
        void execute(std::shared_ptr<simplepubsub::IPublisher> ptr) override;

    private:
        struct createFileData
        {
            std::string id;
            std::string redius;
            std::string height;
        };
        struct createFileData data;
    };

    class ProbeCommand : public ICommand
    {
    public:
        ProbeCommand(std::string _sensor_data): sensor_data(_sensor_data){};
        void execute(std::shared_ptr<simplepubsub::IPublisher> ptr) override;

    private:
        std::string sensor_data;
    };

    class StopCommand : public ICommand
    {
    public:
        StopCommand(){};
        void execute(std::shared_ptr<simplepubsub::IPublisher> ptr) override;
    };

    class GetInfoCommand : public ICommand
    {
    public:
        GetInfoCommand(){};
        void execute(std::shared_ptr<simplepubsub::IPublisher> ptr) override;
    };

    class CommandFactory
    {
    public:
        static std::unique_ptr<ICommand> createCommand(const nlohmann::json &commandJson)
        {
            auto commandType = commandJson["Command"].get<int>();
            if (commandType == 0) // CREATE
            {
                auto args = commandJson["Args"];
                try
                {
                    return std::make_unique<CreateCommand>(args[0], args[1], args[2]);
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

    class CommandInvoker
    {
    public:
        CommandInvoker(std::shared_ptr<simplepubsub::IPublisher> ptr) : _publisher_ptr(ptr) {}
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

        ~CommandInvoker()
        {
            _publisher_ptr.reset();
        }

    private:
        std::shared_ptr<simplepubsub::IPublisher> _publisher_ptr;
    };

}
#endif /* COMMANDSOLVER_H */
