# ZED_Server2

## Introduction

ZED_Server2 is a WebSocket server designed to facilitate local network communication between a controller and a location processing system. It receives commands from an external controller and interacts with the ZED system to record positional data in real-time. This server is an integral part of a larger system that aims to provide accurate and real-time location tracking.

## Building and Running

To build and run ZED_Server2, you need to have [CMake](https://cmake.org/) and the necessary dependencies installed. Follow these steps:

### Pre-Build
ZED_Server2 depend on [ZeroMQ](https://zeromq.org/), an open-source universal messaging library.

The Library install steps:

* Install ZeroMQ (libzmq)
    ```bash
    git clone https://github.com/zeromq/libzmq.git
    cd libzmq
    mkdir build
    cd build
    cmake ..
    sudo make -j4 install
    ```
* Install cppzmq:
    ```bash
    git clone https://github.com/zeromq/cppzmq.git
    cd cppzmq
    mkdir build
    cd build
    cmake ..
    sudo make -j4 install
    ```
### Test Dependency
* Install gmock:
    ```bash
    sudo apt install -y libgmock-dev
    ```
### For deploy environment build (Jetson Nano with ZED SDK)

```bash
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release -DUSE_DEPLOYMENT_ENV=ON ..
make -j4
./ZED_Server2
```

### For develop environment build (without ZED SDK) and test

```bash
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release -DUSE_DEPLOYMENT_ENV=OFF ..
make -j4 && ctest
```

### For develop environment build (without ZED SDK)

```bash
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Coverage -DUSE_DEPLOYMENT_ENV=OFF ..
make -j4 && ctest
make coverage
```

## File Tree

Below is the file tree structure of the ZED_Server2 project, outlining the main components:

```
ZED_Server2/
│
├── source/                 # Source files
|   ├── CommandSolver/      # Solve the input Command 
|   ├── PositionTracker/    # Use ZED to track the position
│   ├── SocketServer/       # Socket Server
|   └── main.cpp            # Main application entry point
|
├── include/                # Header files
│
├── tests/                  # Test files
│
├── module/                 # Third-party module
|
├── CMakeLists.txt          # CMake configuration file
├── testClient.py           # Client to test 
├── zedserver2              # doxygen configuration
└── README.md
```

## To-Do List

- [ ] Add all the comments in doxygen style.
    - [X] WebSocket
    - [ ] Socket
    - [ ] SocketServer
    - [ ] ZedPositioner
    - [ ] CommandSolver
    - [ ] PositionTracker
    - [ ] SimplePubSub

## Code Coverage Report

The code coverage report for ZED_Server2 provides an overview of how much of the codebase is tested. Here's a summary of the coverage rates along with detailed metrics for each file.

**Summary:**
- **Lines Covered**: 98.1% (845 of 861 lines)
- **Functions Covered**: 94.1% (348 of 370 functions)
- **Branches**: No data found

**Detailed Coverage:**

| Filename                                   | Lines       | Functions   | Branches    |
|--------------------------------------------|-------------|-------------|-------------|
| **include/**                               |             |             |             |
| CommandSolver.hpp                          | 76.8% (56)  | 90.0% (10)  | -           |
| PositionTracker.hpp                        | 100% (11)   | 75.0% (4)   | -           |
| SimplePubSub.hpp                           | 100% (80)   | 85.0% (20)  | -           |
| Socket.hpp                                 | 100% (4)    | 57.1% (7)   | -           |
| SocketServer.hpp                           | 100% (28)   | 100% (4)    | -           |
| WebSocket.hpp                              | 100% (7)    | 100% (1)    | -           |
| **source/**                                |             |             |             |
| CommandSolver/CommandSolver.cpp            | 100% (15)   | 100% (4)    | -           |
| PositionTracker/PositionTracker.cpp        | 100% (21)   | 100% (3)    | -           |
| SocketServer/SocketServer.cpp              | 100% (92)   | 66.7% (18)  | -           |
| SocketServer/WebSocket.cpp                 | 100% (37)   | 100% (6)    | -           |
| **tests/**                                 |             |             |             |
| commandsolver_test.cc                      | 100% (55)   | 100% (28)   | -           |
| commandsolver_test.h                       | 100% (19)   | 87.5% (8)   | -           |
| positiontracker_test.cc                    | 100% (30)   | 100% (16)   | -           |
| positiontracker_test.h                     | 100% (22)   | 93.3% (15)  | -           |
| simplepubsub_test.cc                       | 100% (23)   | 100% (10)   | -           |
| simplepubsub_test.h                        | 100% (12)   | 83.3% (6)   | -           |
| socketserver_test.cc                       | 100% (179)  | 100% (101)  | -           |
| socketserver_test.h                        | 96.1% (76)  | 89.5% (38)  | -           |
| websocket_test.cc	                         | 100% (77)   | 100% (52)   | -           |
| websocket_test.h                           | 100% (17)   | 94.7% (19)  | -           |
---