cmake_minimum_required(VERSION 3.14)
project(IoTSimulation VERSION 1.0.0 LANGUAGES CXX)


set(CMAKE_CXX STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")

include_directories(include)

file(GLOB_RECURSE SOURCES "src/*.cpp")
add_executable(iot_simulation ${SOURCES})


add_library(iot_simulation_lib ${SOURCES})
target_include_directories(iot_simulation_lib PUBLIC include)

enable_testing()

add_subdirectory(tests)

install(TARGETS iot_simulation DESTINATION bin)
install(DIRECTORY include/ DESTINATION include)
add_executable(simulation_test test/simulation_test.cpp)

# Link libraries for test
target_link_libraries(simulation_test iot_simulation_lib pthread)

# Make sure the test can find our headers
target_include_directories(simulation_test PRIVATE include)
# Add this line to find all source files, or explicitly list them:
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
    src/utils/ConfigManager.cpp  # Add this line
     src/utils/PerformanceMonitor.cpp  # Add this line
    src/security/SecurityManager.cpp   # Add this line

)