# tests/CMakeLists.txt
find_package(GTest QUIET)

if(GTest_FOUND)
    enable_testing()
    
    # Add test executable
    add_executable(iot_tests 
        test_main.cpp
        core/test_IoTDevice.cpp
        core/test_Message.cpp
        devices/test_Sensor.cpp
        devices/test_Actuator.cpp
    )
    
    target_link_libraries(iot_tests GTest::GTest GTest::Main iot_simulation_lib)
    
    # Add tests
    add_test(NAME IoTDeviceTests COMMAND iot_tests)
    
else()
    message(WARNING "Google Test not found. Tests will not be built.")
endif()