cmake_minimum_required(VERSION 3.14)
PROJECT(ZED_Server2)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_options(-Wall)

# Check if the coverage configuration is defined
if(CMAKE_BUILD_TYPE MATCHES "Coverage")
    # Add compile options for coverage instrumentation
    add_compile_options(--coverage)

    # Link the coverage library
    link_libraries(gcov)
elseif(NOT DEFINED CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Custom target for running lcov
add_custom_target(coverage
    COMMAND lcov --directory . --capture --output-file coverage.info                                # Capture coverage info
    COMMAND lcov --remove coverage.info '/usr/*' --output-file coverage.info                        # Filter out system libraries
    COMMAND lcov --remove coverage.info '${CMAKE_BINARY_DIR}/_deps/**' --output-file coverage.info  # Filter out third-party libraries
    COMMAND lcov --list coverage.info                                                               # Debug info
    COMMAND genhtml coverage.info --output-directory coverage                                       # Generate HTML report
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}                                                           # Working directory
)

option(USE_DEPLOYMENT_ENV "Build configuration for deployment" OFF)

# include modules
add_subdirectory(module)


if(USE_DEPLOYMENT_ENV)
    # ZED Configuration
    add_compile_options(-D USE_DEPLOYMENT_ENV)
    option(LINK_SHARED_ZED "Link with the ZED SDK shared executable" ON)
    find_package(ZED 3 REQUIRED)
    find_package(CUDA ${ZED_CUDA_VERSION} REQUIRED)

    include_directories(${ZED_INCLUDE_DIRS})
    include_directories(${CUDA_INCLUDE_DIRS})

    if (LINK_SHARED_ZED)
        SET(ZED_LIBS ${ZED_LIBRARIES} ${CUDA_CUDA_LIBRARY} ${CUDA_CUDART_LIBRARY})
    else()
        SET(ZED_LIBS ${ZED_STATIC_LIBRARIES} ${CUDA_CUDA_LIBRARY} ${CUDA_LIBRARY})
    endif()
endif()

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/)

# Add source files
file(GLOB_RECURSE SOCKETSERVER_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/source/SocketServer/*.cpp)
file(GLOB_RECURSE COMMANDSOLVER_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/source/CommandSolver/*.cpp)

if(USE_DEPLOYMENT_ENV)
    file(GLOB_RECURSE POSIONTRACKER_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/source/PositionTracker/*.cpp)
else()
    set(POSIONTRACKER_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/source/PositionTracker/PositionTracker.cpp)
endif()

# Add the library
add_library(SocketServer STATIC ${SOCKETSERVER_SRC_FILES})
add_library(CommandSolver STATIC ${COMMANDSOLVER_SRC_FILES})
add_library(PositionTracker STATIC ${POSIONTRACKER_SRC_FILES})


# Add the executable
add_executable(${PROJECT_NAME} 
    ${COMMANDSOLVER_SRC_FILES} 
    ${SOCKETSERVER_SRC_FILES} 
    ${SIMPLEPUBSUB_SRC_FILES}
    ${POSIONTRACKER_SRC_FILES}
    ${CMAKE_CURRENT_SOURCE_DIR}/source/main.cpp)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
    SocketServer
    PositionTracker
    zmq
    -lm
    -lpthread)

target_link_libraries(CommandSolver
                        PUBLIC
                        nlohmann_json::nlohmann_json)
target_link_libraries(SocketServer
                        PUBLIC
                        CommandSolver
                        PositionTracker)

if(USE_DEPLOYMENT_ENV)
    target_link_libraries(PositionTracker
                            PUBLIC
                            nlohmann_json::nlohmann_json
                            ${ZED_LIBS})       
else()
    target_link_libraries(PositionTracker
                            PUBLIC
                            nlohmann_json::nlohmann_json)                             
endif()

option(BUILD_TESTING "Build the testing tree." ON)
if(BUILD_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif()