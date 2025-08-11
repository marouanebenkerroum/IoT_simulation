#include <iostream>
#include <memory>
#include <thread>
#include <chrono>
#include "../include/core/DeviceManager.h"
#include "../include/network/NetworkManager.h"
#include "../include/core/Message.h"
#include "../include/devices/ConcreteSensors.h"
#include "../include/devices/ConcreteActuators.h"

int main() {
    std::cout << "IoT Network Simulation - Day 3 Implementation" << std::endl;
    std::cout << "===============================================" << std::endl;
    
    try {
        // Create device manager
        auto deviceManager = std::make_shared<iot::DeviceManager>();
        std::cout << "Device manager created" << std::endl;
        
        // Create network manager
        auto networkManager = std::make_shared<iot::NetworkManager>(deviceManager);
        std::cout << "Network manager created" << std::endl;
        
        // Start network manager
        networkManager->start();
        
        // Create and register devices
        std::cout << "\n=== Creating and Registering Devices ===" << std::endl;
        
        // Sensors
        auto tempSensor = std::make_shared<iot::TemperatureSensor>("TEMP_001", "Living Room Temperature");
        auto humiditySensor = std::make_shared<iot::HumiditySensor>("HUM_001", "Basement Humidity");
        auto motionSensor = std::make_shared<iot::MotionSensor>("MOTION_001", "Front Door Motion");
        
        // Actuators
        auto led = std::make_shared<iot::LED>("LED_001", "Status LED");
        auto motor = std::make_shared<iot::Motor>("MOTOR_001", "Ventilation Motor");
        auto relay = std::make_shared<iot::Relay>("RELAY_001", "Water Pump Relay");
        
        // Register devices
        deviceManager->registerDevice(tempSensor);
        deviceManager->registerDevice(humiditySensor);
        deviceManager->registerDevice(motionSensor);
        deviceManager->registerDevice(led);
        deviceManager->registerDevice(motor);
        deviceManager->registerDevice(relay);
        
        // List registered devices
        deviceManager->listDevices();
        
        std::cout << "\n=== Testing Network Communication ===" << std::endl;
        
        // Test direct device communication
        std::cout << "Sending temperature reading to LED..." << std::endl;
        iot::Message tempMsg("TEMP_001", "LED_001", "Temperature: 23.5°C", iot::Message::MessageType::DATA);
        networkManager->sendMessage(tempMsg);
        
        // Test command to motor
        std::cout << "Sending command to motor..." << std::endl;
        iot::Message motorCmd("CONTROLLER", "MOTOR_001", "ON", iot::Message::MessageType::COMMAND);
        networkManager->sendMessage(motorCmd);
        
        // Test broadcast message
        std::cout << "Broadcasting system status..." << std::endl;
        iot::Message broadcastMsg("SYSTEM", "ALL", "System maintenance in 1 hour", iot::Message::MessageType::DATA);
        networkManager->broadcastMessage(broadcastMsg);
        
        // Simulate some network activity
        std::cout << "\n=== Simulating Network Activity ===" << std::endl;
        networkManager->setNetworkConditions(0.1, 10.0, 50.0);  // 10% packet loss, 10-50ms delay
        
        // Send multiple messages
        for (int i = 0; i < 5; i++) {
            std::string payload = "Test message " + std::to_string(i + 1);
            iot::Message testMsg("SYSTEM", "LED_001", payload, iot::Message::MessageType::DATA);
            networkManager->sendMessage(testMsg);
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        
        // Wait a bit for message processing
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        
        // Print network statistics
        networkManager->printStats();
        
        // Stop network manager
        networkManager->stop();
        
        std::cout << "\nDay 3 implementation completed successfully!" << std::endl;
        std::cout << "Successfully implemented:" << std::endl;
        std::cout << "- Device Manager with registration system" << std::endl;
        std::cout << "- Network Manager with message queuing" << std::endl;
        std::cout << "- Network protocols and statistics" << std::endl;
        std::cout << "- Network failure simulation" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}