# CMakeLists.txt
cmake_minimum_required(VERSION 3.14)
project(IoTSimulation VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -pthread")

# Include directories
include_directories(include)

# Explicitly list source files
set(SOURCES
    src/main.cpp
    src/core/IoTDevice.cpp
    src/core/Message.cpp
    src/core/DeviceManager.cpp
    src/devices/Sensor.cpp
    src/devices/Actuator.cpp
    src/devices/ConcreteSensors.cpp
    src/devices/ConcreteActuators.cpp
    src/network/NetworkManager.cpp
)

# Create executable
add_executable(iot_simulation ${SOURCES})

# Create library (optional)
add_library(iot_simulation_lib ${SOURCES})
target_include_directories(iot_simulation_lib PUBLIC include)

# Link pthread library
target_link_libraries(iot_simulation pthread)
target_link_libraries(iot_simulation_lib pthread)

# Installation rules
install(TARGETS iot_simulation DESTINATION bin)
install(DIRECTORY include/ DESTINATION include)