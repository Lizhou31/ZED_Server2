#include "commandsolver_test.h"

TEST_F(CommandSolverTest, CreateCommandSuccessDecode)
{
    auto command = testJson["CREATE"];
    auto successCommand = command[0];
    ASSERT_EQ(0, successCommand["Command"]);
    auto createcommand = commandsolver::CommandFactory::createCommand(successCommand);
    commandsolver::CreateCommand *ptr = dynamic_cast<commandsolver::CreateCommand *>(createcommand.get());

    EXPECT_EQ("test", getcreateFileDdata(ptr).id);
    EXPECT_EQ("10", getcreateFileDdata(ptr).redius);
    EXPECT_EQ("15", getcreateFileDdata(ptr).height);
}

TEST_F(CommandSolverTest, CreateCommandFailedLackArgs)
{
    auto command = testJson["CREATE"];
    auto failedCommand = command[1];
    ASSERT_EQ(0, failedCommand["Command"]);
    EXPECT_THROW({
        try
        {
            auto createcommand = commandsolver::CommandFactory::createCommand(failedCommand);
        }
        catch (const std::exception &e)
        {
            throw;
        }
    },
                 std::exception);
}

TEST_F(CommandSolverTest, CreateCommandExecuteSuccess)
{
    // Publish the file name to "CreateFile Topic"
    // TODO: Design the create file name logic.
    auto command = testJson["CREATE"];
    auto successCommand = command[0];
    ASSERT_EQ(0, successCommand["Command"]);
    auto args = successCommand["Args"];
    
    std::string data = nlohmann::json{{"id", args[0]},
                                      {"redius", args[1]},
                                      {"height", args[2]}}
                           .dump();

    EXPECT_CALL(*mock_publish, publish("CreateFile", data)).Times(1);
    invoker->executeCommand(successCommand);
}