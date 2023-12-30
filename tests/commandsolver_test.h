#include "CommandSolver.hpp"
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <fstream>

class MockPublish : public simplepubsub::IPublisher
{
public:
    MOCK_METHOD(void, publish, (const std::string &topic, const std::string &data), (override));
};

class CommandSolverTest : public ::testing::Test
{
protected:
    std::unique_ptr<commandsolver::CommandInvoker> invoker;
    nlohmann::json testJson;
    std::shared_ptr<MockPublish> mock_publish;
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
        
        mock_publish = std::make_shared<MockPublish>();
        invoker = std::make_unique<commandsolver::CommandInvoker>(mock_publish);
    }

    void TearDown() override
    {
    }

    commandsolver::CreateCommand::createFileData &getcreateFileDdata(commandsolver::CreateCommand *ptr)
    {
        return ptr->data;
    }
};