# adding Google Test
set(GOOGLETEST_PATH ~/local/googletest)
set(GTEST_INCLUDE_DIR ~/local/include/)
set(GTEST_LIBRARY ~/local/lib/)
set(GTEST_MAIN_LIBRARY ~/local/lib/)
find_package(GTest REQUIRED)

# Add test executable
add_executable(websocket_test ${CMAKE_CURRENT_SOURCE_DIR}/websocket_test.cc )
target_link_libraries(
    websocket_test
    PRIVATE
    GTest::gtest_main
    GTest::gmock
    SocketServer
)
add_executable(socketserver_test ${CMAKE_CURRENT_SOURCE_DIR}/socketserver_test.cc )
target_link_libraries(
    socketserver_test
    PRIVATE
    GTest::gtest_main
    GTest::gmock
    SocketServer
)

# Include the directory
target_include_directories(websocket_test PRIVATE ../include/)
target_include_directories(socketserver_test PRIVATE ../include/)

# Add teh test to be run
gtest_discover_tests(websocket_test)
gtest_discover_tests(socketserver_test)
