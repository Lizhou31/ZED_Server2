/**
 * @file commandsolver_test.h
 * @author Lizhou (lisie31s@gmail.com)
 * @brief commandsolver test header file
 * @version 0.1
 * @date 2024-01-15
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "CommandSolver.hpp"
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <fstream>

/**
 * @class MockPublish
 * 
 * @brief Mock class for IPublisher
 */
class MockPublish : public simplepubsub::IPublisher
{
public:
    MOCK_METHOD(void, publish, (const std::string &topic, const std::string &data), (override));
};

/**
 * @class CommandSolverTest
 * 
 * @brief CommandSolver test class declaration
 */
class CommandSolverTest : public ::testing::Test
{
protected:

    /**
     * @brief invoker smart pointer
     */
    std::unique_ptr<commandsolver::CommandInvoker> invoker;

    /**
     * @brief test json data
     */
    nlohmann::json testJson;

    /**
     * @brief mock publish smart pointer
     */
    std::shared_ptr<MockPublish> mock_publish;
    
    /**
     * @brief Construct a new Command Solver Test object
     */
    CommandSolverTest() {}

    /**
     * @brief Destroy the Command Solver Test object
     */
    ~CommandSolverTest() override
    {
    }

    /**
     * @brief Set up the Command Solver Test object
     */
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

    /**
     * @brief Tear down the Command Solver Test object
     */
    void TearDown() override
    {
    }

    /**
     * @brief Get the createFileDdata object
     * 
     * @param ptr create command pointer
     * @return commandsolver::CreateCommand::createFileData& 
     */
    commandsolver::CreateCommand::createFileData &getcreateFileDdata(commandsolver::CreateCommand *ptr)
    {
        return ptr->data;
    }
};