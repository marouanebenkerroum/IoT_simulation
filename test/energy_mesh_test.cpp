#include <iostream>
#include <memory>
#include <thread>
#include <chrono>
#include "../include/devices/BatterySensors.h"
#include "../include/network/MeshNetwork.h"

int main() {
    std::cout << "=========================================" << std::endl;
    std::cout << "Energy Management & Mesh Network Test" << std::endl;
    std::cout << "=========================================" << std::endl;
    
    try {
        // Test Battery Device Functionality
        std::cout << "\n1. Testing Battery Device Functionality..." << std::endl;
        
        auto batteryTempSensor = std::make_shared<iot::BatteryTemperatureSensor>("BATT_TEMP_001", "Battery Temperature Sensor");
        auto batteryMotionSensor = std::make_shared<iot::BatteryMotionSensor>("BATT_MOTION_001", "Battery Motion Sensor");
        
        std::cout << "Initial Status:" << std::endl;
        std::cout << batteryTempSensor->getStatus() << std::endl;
        std::cout << batteryMotionSensor->getStatus() << std::endl;
        
        // Test battery consumption
        std::cout << "\nTesting battery consumption..." << std::endl;
        for (int i = 0; i < 5; ++i) {
            double temp = batteryTempSensor->readValue();
            batteryTempSensor->sendData();
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        
        std::cout << "After 5 readings:" << std::endl;
        std::cout << batteryTempSensor->getStatus() << std::endl;
        
        // Test low power mode
        std::cout << "\nTesting low power mode..." << std::endl;
        batteryTempSensor->rechargeBattery(50.0);  // Recharge to test low power
        std::cout << "After recharge: " << batteryTempSensor->getStatus() << std::endl;
        
        // Test Mesh Network Functionality
        std::cout << "\n\n2. Testing Mesh Network Functionality..." << std::endl;
        
        iot::MeshNetwork meshNetwork(10);
        
        // Add devices to mesh
        meshNetwork.addDevice("GATEWAY_01", true);  // Gateway
        meshNetwork.addDevice("SENSOR_01");
        meshNetwork.addDevice("SENSOR_02");
        meshNetwork.addDevice("SENSOR_03");
        meshNetwork.addDevice("SENSOR_04");
        
        // Add neighbor relationships
        meshNetwork.addNeighbor("SENSOR_01", "GATEWAY_01");
        meshNetwork.addNeighbor("SENSOR_02", "SENSOR_01");
        meshNetwork.addNeighbor("SENSOR_03", "SENSOR_02");
        meshNetwork.addNeighbor("SENSOR_04", "SENSOR_03");
        
        // Print topology
        meshNetwork.printTopology();
        
        // Test path finding
        std::cout << "\nTesting path finding..." << std::endl;
        auto path1 = meshNetwork.findOptimalPath("SENSOR_04");
        std::cout << "Path from SENSOR_04 to GATEWAY_01: ";
        for (size_t i = 0; i < path1.size(); ++i) {
            std::cout << path1[i];
            if (i < path1.size() - 1) std::cout << " -> ";
        }
        std::cout << std::endl;
        
        std::cout << "\nHop counts:" << std::endl;
        std::cout << "SENSOR_01: " << meshNetwork.getHopCount("SENSOR_01") << " hops" << std::endl;
        std::cout << "SENSOR_02: " << meshNetwork.getHopCount("SENSOR_02") << " hops" << std::endl;
        std::cout << "SENSOR_03: " << meshNetwork.getHopCount("SENSOR_03") << " hops" << std::endl;
        std::cout << "SENSOR_04: " << meshNetwork.getHopCount("SENSOR_04") << " hops" << std::endl;
        
        // Print statistics
        meshNetwork.printStatistics();
        
        std::cout << "\n=========================================" << std::endl;
        std::cout << "Energy Management & Mesh Network Test COMPLETED!" << std::endl;
        std::cout << "=========================================" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error in test: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}