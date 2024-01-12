#include "positiontracker_test.h"

TEST_F(PositionTrackerTest, testPubRepeatedly)
{
    EXPECT_CALL(*positioner, getStatus(::testing::_)).WillRepeatedly(::testing::Invoke([this](int &status)
                                                                                       { status = 3; }));

    EXPECT_CALL(*positioner, getPosition(::testing::_, ::testing::_, ::testing::_)).WillRepeatedly(::testing::Invoke([this](int &x, int &y, int &z)
                                                                                                                     {  x = 1;
                                                                                                                        y = 2;
                                                                                                                        z = 3; }));

    EXPECT_CALL(*pub_ptr, publish("ZED_Status", ::testing::_)).WillRepeatedly(::testing::Invoke([this](const std::string &topic, const std::string &data)
                                                                                                {   status_ok.store(1); 
                                                                                                    nlohmann::json d = nlohmann::json::parse(data);
                                                                                                    EXPECT_EQ(d["Status"].get<int>(), 3); }));

    EXPECT_CALL(*pub_ptr, publish("ZED_Position", ::testing::_)).WillRepeatedly(::testing::Invoke([this](const std::string &topic, const std::string &data)
                                                                                                  { position_ok.store(1);
                                                                                                    nlohmann::json d = nlohmann::json::parse(data);
                                                                                                    EXPECT_EQ(d["X"].get<int>(), 1);
                                                                                                    EXPECT_EQ(d["Y"].get<int>(), 2);
                                                                                                    EXPECT_EQ(d["Z"].get<int>(), 3); }));

    std::unique_ptr<MockPositioner> ptr(positioner);
    std::unique_ptr<MockPublish>_pub_ptr(pub_ptr);
    positionTracker = std::make_unique<positiontracker::PositionTracker>(std::move(_pub_ptr), std::move(ptr));

    while (status_ok.load() == 0 || position_ok.load() == 0)
        ;
    EXPECT_EQ(get_status(), 3);
    EXPECT_EQ(get_positionX(), 1);
    EXPECT_EQ(get_positionY(), 2);
    EXPECT_EQ(get_positionZ(), 3);
}

TEST_F(PositionTrackerTest, testpackStatus)
{
    // TODO:
}

TEST_F(PositionTrackerTest, testpackPosition)
{
    // TODO:
}