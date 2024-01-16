/**
 * @file commandsolver_test.cc
 * @author Lizhou (lisie31s@gmail.com)
 * @brief commandsolver test source file
 * @version 0.1
 * @date 2024-01-15
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "commandsolver_test.h"

/**
 * @brief CreateCommandSuccessDecode test
 */
TEST_F(CommandSolverTest, CreateCommandSuccessDecode)
{
    auto command = testJson["CREATE"];
    auto successCommand = command[0];
    ASSERT_EQ(0, successCommand["Command"]);
    auto createcommand = commandsolver::CommandFactory::createCommand(successCommand);
    commandsolver::CreateCommand *ptr = dynamic_cast<commandsolver::CreateCommand *>(createcommand.get());

    EXPECT_EQ("test", getcreateFileDdata(ptr).id);
    EXPECT_EQ("10", getcreateFileDdata(ptr).radius);
    EXPECT_EQ("15", getcreateFileDdata(ptr).height);
}

/**
 * @brief ProbeCommandSuccessDecode test
 */
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

/**
 * @brief ProbeCommandSuccessDecode test
 */
TEST_F(CommandSolverTest, CreateCommandExecuteSuccess)
{
    auto command = testJson["CREATE"];
    auto successCommand = command[0];
    ASSERT_EQ(0, successCommand["Command"]);
    auto args = successCommand["Args"];

    std::string data = nlohmann::json{{"id", (args[0])["ID"].get<std::string>()},
                                      {"radius", (args[1])["Radius"].get<std::string>()},
                                      {"height", (args[2])["Height"].get<std::string>()}}
                           .dump();

    EXPECT_CALL(*mock_publish, publish("CreateFile", data)).Times(1);
    invoker->executeCommand(successCommand.dump());
}

/**
 * @brief ProbeCommandSuccessDecode test
 */
TEST_F(CommandSolverTest, ProbeCommandExecuteSuccess)
{
    auto command = testJson["PROBE"];
    auto successCommand = command[0];
    ASSERT_EQ(1, successCommand["Command"]);
    auto args = successCommand["Args"][0].get<std::string>();

    EXPECT_CALL(*mock_publish, publish("Probe", args)).Times(1);

    invoker->executeCommand(successCommand.dump());
}

/**
 * @brief StopCommandExecuteSuccess test
 */
TEST_F(CommandSolverTest, StopCommandExecuteSuccess)
{
    auto command = testJson["STOP"];
    auto successCommand = command[0];
    ASSERT_EQ(4, successCommand["Command"]);

    EXPECT_CALL(*mock_publish, publish("StopTest", ::testing::_)).Times(1);

    invoker->executeCommand(successCommand.dump());
}

/**
 * @brief GetInfoCommandExecuteSuccess test
 */
TEST_F(CommandSolverTest, GetInfoCommandExecuteSuccess)
{
    auto command = testJson["GETINFO"];
    auto successCommand = command[0];
    ASSERT_EQ(5, successCommand["Command"]);

    EXPECT_CALL(*mock_publish, publish("GetInfo", ::testing::_)).Times(1);

    invoker->executeCommand(successCommand.dump());
}

/**
 * @brief CommandExecuteFailed test
 */
TEST_F(CommandSolverTest, CommandExecuteFailed)
{
    EXPECT_THROW({
        try
        {
            invoker->executeCommand("Test");
        }
        catch (const std::invalid_argument &e)
        {
            EXPECT_STREQ("Input command error", e.what());
            throw;
        }
    },
                 std::invalid_argument);
}