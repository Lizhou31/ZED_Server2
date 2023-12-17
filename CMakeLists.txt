cmake_minimum_required(VERSION 3.14)
PROJECT(ZED_Server2)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE "Debug")

# ZED Configuration
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

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/)

# Add source files
file(GLOB_RECURSE SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp)

# Add the library

add_library(SocketServer STATIC ${SRC_FILES})

# Add the executable
add_executable(${PROJECT_NAME} ${SRC_FILES})

target_link_libraries(${PROJECT_NAME}
                        ${ZED_LIBS}
                        -lm)

enable_testing()
add_subdirectory(tests)