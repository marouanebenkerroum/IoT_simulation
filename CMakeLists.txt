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
    src/devices/BatteryManager.cpp          # Add this line
    src/devices/BatterySensors.cpp         # Add this line
    src/network/NetworkManager.cpp
    src/network/MeshNetwork.cpp            # Add this line
    src/simulation/SimulationEngine.cpp
    src/utils/ConfigManager.cpp
    src/security/SecurityManager.cpp
    src/security/IPSecManager.cpp
)
add_executable(ipsec_test test/ipsec_test.cpp)
target_link_libraries(ipsec_test iot_simulation_lib pthread)
target_include_directories(ipsec_test PRIVATE include)

add_executable(enhanced_simulation_test test/enhanced_simulation_test.cpp)
target_link_libraries(enhanced_simulation_test iot_simulation_lib pthread)
target_include_directories(enhanced_simulation_test PRIVATE include)

add_executable(energy_mesh_test test/energy_mesh_test.cpp)
target_link_libraries(energy_mesh_test iot_simulation_lib pthread)
target_include_directories(energy_mesh_test PRIVATE include)