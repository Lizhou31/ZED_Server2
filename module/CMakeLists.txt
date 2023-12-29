set(cppzmq_DIR "/usr/local/share/cmake/cppzmq")
find_package(cppzmq REQUIRED)

include(FetchContent)

FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
)

FetchContent_MakeAvailable(json)