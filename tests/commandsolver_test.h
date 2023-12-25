#include "CommandSolver.hpp"
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <fstream>

class MockCreateCommand : public commandsolver::CreateCommand
{
    MOCK_METHOD(void, execute, (), (override));
};

class CommandSolverTest : public ::testing::Test
{
protected:
    commandsolver::CommandInvoker invoker;
    nlohmann::json testJson;
    CommandSolverTest() {}
    ~CommandSolverTest() override
    {
    }
    void SetUp() override
    {
        std::ifstream file("test_data/test_command.json");
        ASSERT_TRUE(file.is_open());

        std::stringstream buffer;
        buffer << file.rdbuf();
        file.close();

        testJson = nlohmann::json::parse(buffer.str());
    }

    void TearDown() override
    {
    }

    std::string &getCreateHeight(commandsolver::CreateCommand *ptr){
        return ptr->height;
    }

    std::string &getCreateRedius(commandsolver::CreateCommand  *ptr){
        return ptr->redius;
    }

    std::string &getCreateID(commandsolver::CreateCommand *ptr){
        return ptr->id;
    }
};