#include <iostream>
#include <memory>
#include <thread>
#include <chrono>
#include "../include/core/DeviceManager.h"
#include "../include/network/NetworkManager.h"
#include "../include/network/ProtocolCharacteristics.h"
#include "../include/core/Message.h"
#include "../include/devices/ConcreteSensors.h"
#include "../include/devices/ConcreteActuators.h"
#include "../include/devices/ProtocolSensors.h"

int main() {
    std::cout << "IoT Network Simulation - Day 3 Enhanced with Protocol Support" << std::endl;
    std::cout << "===========================================================" << std::endl;
    
    try {
        // Create device manager
        auto deviceManager = std::make_shared<iot::DeviceManager>();
        auto networkManager = std::make_shared<iot::NetworkManager>(deviceManager);
        
        std::cout << "Core components created" << std::endl;
        
        // Create and register regular devices first
        std::cout << "\n=== Creating Regular Devices ===" << std::endl;
        
        // Sensors
        auto tempSensor = std::make_shared<iot::TemperatureSensor>("TEMP_001", "Living Room Temperature");
        auto humiditySensor = std::make_shared<iot::HumiditySensor>("HUM_001", "Basement Humidity");
        auto motionSensor = std::make_shared<iot::MotionSensor>("MOTION_001", "Front Door Motion");
        
        // Actuators
        auto led = std::make_shared<iot::LED>("LED_001", "Status LED");
        auto motor = std::make_shared<iot::Motor>("MOTOR_001", "Ventilation Motor");
        auto relay = std::make_shared<iot::Relay>("RELAY_001", "Water Pump Relay");
        
        // Create protocol-aware devices
        std::cout << "\n=== Creating Protocol-Aware Devices ===" << std::endl;
        
        // LoRa Temperature Sensor
        auto loraSensor = std::make_shared<iot::LoRaTemperatureSensor>("LORA_TEMP_001", "Field Temperature");
        networkManager->setDeviceProtocol("LORA_TEMP_001", iot::NetworkManager::Protocol::LORA);
        
        // ZigBee Motion Sensor
        auto zigbeeSensor = std::make_shared<iot::ZigBeeMotionSensor>("ZIGBEE_MOTION_001", "Living Room Motion");
        networkManager->setDeviceProtocol("ZIGBEE_MOTION_001", iot::NetworkManager::Protocol::ZIGBEE);
        
        // BLE Health Sensor
        auto bleSensor = std::make_shared<iot::BLEHealthSensor>("BLE_HEALTH_001", "Wearable Heart Rate");
        networkManager->setDeviceProtocol("BLE_HEALTH_001", iot::NetworkManager::Protocol::BLUETOOTH_LE);
        
        // Register all devices
        deviceManager->registerDevice(tempSensor);
        deviceManager->registerDevice(humiditySensor);
        deviceManager->registerDevice(motionSensor);
        deviceManager->registerDevice(led);
        deviceManager->registerDevice(motor);
        deviceManager->registerDevice(relay);
        deviceManager->registerDevice(loraSensor);
        deviceManager->registerDevice(zigbeeSensor);
        deviceManager->registerDevice(bleSensor);
        
        // List registered devices
        deviceManager->listDevices();
        
        // Set network conditions for different protocols
        std::cout << "\n=== Setting Network Conditions ===" << std::endl;
        
        // Start network manager
        networkManager->start();
        
        // Test regular device communication
        std::cout << "\n=== Testing Regular Device Communication ===" << std::endl;
        iot::Message tempMsg("TEMP_001", "LED_001", "Temperature: 23.5°C", iot::Message::MessageType::DATA);
        networkManager->sendMessage(tempMsg);
        
        // Test protocol-aware device functionality
        std::cout << "\n=== Testing Protocol-Aware Device Features ===" << std::endl;
        
        // Test LoRa sensor battery consumption
        std::cout << "LoRa Sensor Device ID: " << loraSensor->getDeviceId() << std::endl;
        std::cout << "LoRa Sensor Battery: " << loraSensor->getBatteryLevel() << "%" << std::endl;
        for (int i = 0; i < 5; i++) {
            loraSensor->readValue();
            std::cout << "After reading " << (i+1) << ": " << loraSensor->getBatteryLevel() << "%" << std::endl;
        }
        
        // Test ZigBee sensor with mesh routing
        dynamic_cast<iot::ZigBeeMotionSensor*>(zigbeeSensor.get())->setHopCount(3);
        std::cout << "ZigBee Sensor Battery: " << zigbeeSensor->getBatteryLevel() << "%" << std::endl;
        zigbeeSensor->sendData();
        std::cout << "After transmission: " << zigbeeSensor->getBatteryLevel() << "%" << std::endl;
        
        // Test BLE sensor
        std::cout << "BLE Sensor Battery: " << bleSensor->getBatteryLevel() << "%" << std::endl;
        bleSensor->readValue();
        std::cout << "After reading: " << bleSensor->getBatteryLevel() << "%" << std::endl;
        
        // Demonstrate protocol characteristics
        std::cout << "\n=== Protocol Characteristics ===" << std::endl;
        auto loraChar = iot::getProtocolCharacteristics(iot::NetworkManager::Protocol::LORA);
        auto zigbeeChar = iot::getProtocolCharacteristics(iot::NetworkManager::Protocol::ZIGBEE);
        auto bleChar = iot::getProtocolCharacteristics(iot::NetworkManager::Protocol::BLUETOOTH_LE);
        
        std::cout << "LoRa: " << loraChar.name 
                  << " (Range: " << loraChar.maxRangeKm << "km, "
                  << "Data Rate: " << loraChar.dataRateKbps << "kbps)" << std::endl;
        std::cout << "ZigBee: " << zigbeeChar.name 
                  << " (Range: " << zigbeeChar.maxRangeKm << "km, "
                  << "Mesh: " << (zigbeeChar.supportsMesh ? "Yes" : "No") << ")" << std::endl;
        std::cout << "BLE: " << bleChar.name 
                  << " (Range: " << bleChar.maxRangeKm << "km, "
                  << "Latency: " << bleChar.latencyMs << "ms)" << std::endl;
        
        // Print final statistics
        networkManager->printStats();
        
        // Stop network manager
        networkManager->stop();
        
        std::cout << "\nDay 3 Enhanced Implementation Completed Successfully!" << std::endl;
        std::cout << "Successfully implemented:" << std::endl;
        std::cout << "- Multiple IoT communication protocols (LoRa, ZigBee, BLE, etc.)" << std::endl;
        std::cout << "- Protocol-specific device behaviors and characteristics" << std::endl;
        std::cout << "- Battery management for low-power devices" << std::endl;
        std::cout << "- Comprehensive protocol characteristics database" << std::endl;
        


        // Comprehensive Protocol Demonstration
std::cout << "\n=== COMPREHENSIVE PROTOCOL DEMONSTRATION ===" << std::endl;

// Show all available protocols
std::cout << "\nAvailable IoT Communication Protocols:" << std::endl;
std::cout << "----------------------------------------" << std::endl;

std::vector<iot::NetworkManager::Protocol> protocols = {
    iot::NetworkManager::Protocol::MQTT,
    iot::NetworkManager::Protocol::COAP,
    iot::NetworkManager::Protocol::HTTP,
    iot::NetworkManager::Protocol::LORA,
    iot::NetworkManager::Protocol::ZIGBEE,
    iot::NetworkManager::Protocol::BLUETOOTH_LE,
    iot::NetworkManager::Protocol::THREAD,
    iot::NetworkManager::Protocol::ZWAVE,
    iot::NetworkManager::Protocol::NB_IOT,
    iot::NetworkManager::Protocol::SIGFOX
};

for (const auto& protocol : protocols) {
    auto characteristics = iot::getProtocolCharacteristics(protocol);
    std::cout << characteristics.name << ":" << std::endl;
    std::cout << "  Range: " << characteristics.maxRangeKm << " km" << std::endl;
    std::cout << "  Data Rate: " << characteristics.dataRateKbps << " kbps" << std::endl;
    std::cout << "  Power Consumption: " << characteristics.powerConsumption << "x" << std::endl;
    std::cout << "  Latency: " << characteristics.latencyMs << " ms" << std::endl;
    std::cout << "  Mesh Support: " << (characteristics.supportsMesh ? "Yes" : "No") << std::endl;
    std::cout << "  Use Case: " << characteristics.typicalUseCase << std::endl;
    std::cout << std::endl;
}

// Demonstrate protocol assignment to devices
std::cout << "Protocol Assignment Examples:" << std::endl;
std::cout << "----------------------------" << std::endl;

networkManager->setDeviceProtocol("TEMP_001", iot::NetworkManager::Protocol::LORA);
networkManager->setDeviceProtocol("HUM_001", iot::NetworkManager::Protocol::ZIGBEE);
networkManager->setDeviceProtocol("LED_001", iot::NetworkManager::Protocol::MQTT);

std::cout << "\nProtocol-aware messaging demonstration:" << std::endl;
std::cout << "--------------------------------------" << std::endl;

// Simulate different network conditions for different protocols
std::cout << "Setting network conditions for LoRa (long-range, low data rate):" << std::endl;
networkManager->setNetworkConditions(0.02, 500.0, 2000.0);  // 2% loss, 500-2000ms delay

iot::Message loraMsg("TEMP_001", "LED_001", "Field Temperature: 23.5°C via LoRa");
networkManager->sendMessage(loraMsg);

std::cout << "\nSetting network conditions for ZigBee (short-range, mesh network):" << std::endl;
networkManager->setNetworkConditions(0.01, 20.0, 100.0);  // 1% loss, 20-100ms delay

iot::Message zigbeeMsg("HUM_001", "LED_001", "Room Humidity: 45% via ZigBee");
networkManager->sendMessage(zigbeeMsg);

std::cout << "\nProtocol Comparison Summary:" << std::endl;
std::cout << "---------------------------" << std::endl;
std::cout << "LoRa:     Long range (15km), Low power, Low data rate (0.3kbps)" << std::endl;
std::cout << "ZigBee:   Short range (100m), Mesh networking, Moderate power" << std::endl;
std::cout << "BLE:      Very short range (50m), Ultra-low latency, Low power" << std::endl;
std::cout << "NB-IoT:   Wide area coverage, Very low power, Cellular-based" << std::endl;
std::cout << "Sigfox:   Ultra-long range (50km), Ultra-low power, Minimal data" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}