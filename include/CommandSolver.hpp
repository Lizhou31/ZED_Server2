#ifndef COMMANDSOLVER_H
#define COMMANDSOLVER_H

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
        virtual void execute() = 0;
    };

    class CreateCommand : public ICommand
    {
        friend class ::CommandSolverTest;

    public:
        CreateCommand(std::string _id, std::string _redius, std::string _height) : id(_id),
                                                                                   redius(_redius),
                                                                                   height(_height)
                                                                                   {
                                                                                        
                                                                                   };
        void execute() override{};

    private:
        std::string id;
        std::string redius;
        std::string height;
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
                try{
                    return std::make_unique<CreateCommand>(args[0], args[1], args[2]);
                }
                catch(std::exception &e){
                    std::cerr << e.what() << std::endl;
                    throw;
                }
            }
            // Handle other commands similarly
            // ...
        }
    };

    class CommandInvoker
    {
    public:
        void executeCommand(const nlohmann::json &commandJson)
        {
            auto command = CommandFactory::createCommand(commandJson);
            if (command)
            {
                command->execute();
            }
        }
    };

}
#endif /* COMMANDSOLVER_H */