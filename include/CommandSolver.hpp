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
        virtual void execute() = 0;
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
        void execute() override{};

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
        ProbeCommand(){};
        void execute(std::shared_ptr<simplepubsub::IPublisher> ptr) override;
        void execute() override {}
    };

    class StopCommand : public ICommand
    {
    public:
        StopCommand(){};
        void execute(std::shared_ptr<simplepubsub::IPublisher> ptr) override;
        void execute() override {}
    };

    class GetInfoCommand : public ICommand
    {
    public:
        GetInfoCommand(){};
        void execute(std::shared_ptr<simplepubsub::IPublisher> ptr) override;
        void execute() override {}
    };

    class CommandFactory
    {
    public:
        static std::unique_ptr<ICommand> createCommand(const nlohmann::json &commandJson)
        {
            auto commandType = commandJson.find("Command");
            if (*commandType == 0) // CREATE
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
            else if (*commandType == 1) // PROBE
            {
                try
                {
                    return std::make_unique<ProbeCommand>();
                }
                catch (const std::exception &e)
                {
                    std::cerr << e.what() << std::endl;
                    throw;
                }
            }
            else if (*commandType == 4) // STOP
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
            else if (*commandType == 5) // GETINFO
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
            // TODO: RESET = 2,
            // TODO: CHANGE = 3
        }
    };

    class CommandInvoker
    {
    public:
        CommandInvoker(std::shared_ptr<simplepubsub::IPublisher> ptr) : _publisher_ptr(ptr) {}
        void executeCommand(const nlohmann::json &commandJson)
        {
            auto command = CommandFactory::createCommand(commandJson);
            if (command)
            {
                command->execute(_publisher_ptr);
            }
        }

    private:
        std::shared_ptr<simplepubsub::IPublisher> _publisher_ptr;
    };

}
#endif /* COMMANDSOLVER_H */
