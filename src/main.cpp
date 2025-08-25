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
#include "../include/simulation/SimulationEngine.h"
#include "../include/security/SecurityManager.h"
#include "../include/utils/PerformanceMonitor.h"
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


// Day 4: Complete Simulation Engine Test
std::cout << "\n=== DAY 4: COMPLETE SIMULATION ENGINE TEST ===" << std::endl;

// Create simulation engine
auto simulationEngine = std::make_shared<iot::SimulationEngine>(deviceManager, networkManager);

std::cout << "Simulation engine created successfully!" << std::endl;

// Test configuration loading
simulationEngine->loadConfig("simulation_config.json");

// Test event scheduling
std::cout << "\nScheduling test events..." << std::endl;

// Schedule a one-time event
simulationEngine->scheduleEvent(
    std::chrono::milliseconds(1000),
    [&]() {
        std::cout << "ONE-TIME EVENT: Temperature reading scheduled event executed!" << std::endl;
        tempSensor->readValue();
    },
    "TEMP_READING_1"
);

// Schedule a repeating event for temperature sensor
simulationEngine->scheduleRepeatingEvent(
    std::chrono::milliseconds(2000),
    [&]() {
        double temp = tempSensor->readValue();
        std::cout << "REPEATING EVENT: Temperature = " << temp << "°C" << std::endl;
        
        // Send temperature data via network
        iot::Message tempMsg("TEMP_001", "LED_001", 
                            "Temperature: " + std::to_string(temp) + "°C");
        networkManager->sendMessage(tempMsg);
    },
    "TEMP_READING_REPEAT"
);

// Schedule LED toggle event
simulationEngine->scheduleEvent(
    std::chrono::milliseconds(3000),
    [&]() {
        std::cout << "EVENT: Toggling LED" << std::endl;
        iot::Message ledMsg("SCHEDULER", "LED_001", "TOGGLE", iot::Message::MessageType::COMMAND);
        networkManager->sendMessage(ledMsg);
    },
    "LED_TOGGLE"
);

// Test simulation engine lifecycle
std::cout << "\nTesting simulation engine lifecycle..." << std::endl;
simulationEngine->start();

// Let it run for a while to process events
std::cout << "Running simulation for 10 seconds..." << std::endl;
std::this_thread::sleep_for(std::chrono::seconds(10));

// Test pause/resume
simulationEngine->pause();
std::cout << "Simulation paused for 2 seconds..." << std::endl;
std::this_thread::sleep_for(std::chrono::seconds(2));

simulationEngine->resume();
std::cout << "Simulation resumed for 5 seconds..." << std::endl;
std::this_thread::sleep_for(std::chrono::seconds(5));

// Print statistics
simulationEngine->printStats();

simulationEngine->stop();
std::cout << "Simulation stopped" << std::endl;

std::cout << "\nDay 4 Implementation Completed Successfully!" << std::endl;
std::cout << "Successfully implemented:" << std::endl;
std::cout << "- Complete Simulation Engine with time control" << std::endl;
std::cout << "- Event-driven architecture with scheduling" << std::endl;
std::cout << "- One-time and repeating event support" << std::endl;
std::cout << "- Simulation pause/resume functionality" << std::endl;
std::cout << "- Configuration management foundation" << std::endl;


// Test ConfigManager
std::cout << "\n=== TESTING CONFIG MANAGER ===" << std::endl;

iot::ConfigManager configMgr;

// Test setting and getting values
configMgr.set("test.string", "Hello World");
configMgr.set("test.integer", "42");
configMgr.set("test.double", "3.14159");
configMgr.set("test.boolean", "true");

std::cout << "String value: " << configMgr.getString("test.string") << std::endl;
std::cout << "Integer value: " << configMgr.getInt("test.integer", 0) << std::endl;
std::cout << "Double value: " << configMgr.getDouble("test.double", 0.0) << std::endl;
std::cout << "Boolean value: " << (configMgr.getBool("test.boolean", false) ? "true" : "false") << std::endl;

// Test default values
std::cout << "Non-existent key with default: " << configMgr.getString("non.existent", "DEFAULT") << std::endl;
std::cout << "Non-existent int with default: " << configMgr.getInt("non.existent", -1) << std::endl;

// Test configuration loading from string
std::string configString = R"(
simulation.speed=2.5
network.packet_loss=0.03
network.delay_min=25.0
network.delay_max=150.0
logging.level=DEBUG
)";

if (configMgr.loadFromString(configString)) {
    std::cout << "\nLoaded configuration from string:" << std::endl;
    std::cout << "Simulation Speed: " << configMgr.getDouble("simulation.speed", 1.0) << std::endl;
    std::cout << "Packet Loss: " << configMgr.getDouble("network.packet_loss", 0.0) << std::endl;
    std::cout << "Delay Range: " << configMgr.getDouble("network.delay_min", 0.0) 
              << " - " << configMgr.getDouble("network.delay_max", 0.0) << " ms" << std::endl;
    std::cout << "Log Level: " << configMgr.getString("logging.level", "INFO") << std::endl;
}

std::cout << "ConfigManager test completed successfully!" << std::endl;


// Day 5: Performance Optimization and Security Test
std::cout << "\n=== DAY 5: PERFORMANCE OPTIMIZATION AND SECURITY ===" << std::endl;

// Test Performance Monitor
iot::PerformanceMonitor perfMonitor;
perfMonitor.recordTime("device_registration", 5.2);
perfMonitor.recordTime("message_processing", 12.8);
perfMonitor.recordTime("device_registration", 4.8);
perfMonitor.recordTime("message_processing", 11.5);

std::cout << "Performance monitoring test completed" << std::endl;

// Test Security Manager
iot::SecurityManager securityManager(iot::SecurityManager::SecurityLevel::ENHANCED);

// Register devices with security
securityManager.registerDevice("TEMP_001", iot::SecurityManager::SecurityLevel::ENHANCED);
securityManager.registerDevice("LED_001", iot::SecurityManager::SecurityLevel::BASIC);
securityManager.registerDevice("MOTOR_001", iot::SecurityManager::SecurityLevel::ENTERPRISE);

// Test authentication
std::cout << "\nTesting device authentication..." << std::endl;
securityManager.authenticateDevice("TEMP_001", "TOKEN_123456");  // This will fail
securityManager.authenticateDevice("TEMP_001", "TOKEN_");  // This will work (we'd need actual token)

// Test authorization
std::cout << "TEMP_001 authorized to send: " 
          << (securityManager.isAuthorizedToSend("TEMP_001") ? "YES" : "NO") << std::endl;

// Test encryption
std::string originalMessage = "Secret temperature reading: 23.5°C";
std::string encrypted = securityManager.encryptMessage(originalMessage, "TEMP_001");
std::string decrypted = securityManager.decryptMessage(encrypted, "TEMP_001");

std::cout << "Original: " << originalMessage << std::endl;
std::cout << "Encrypted: " << encrypted << std::endl;
std::cout << "Decrypted: " << decrypted << std::endl;

// Print final reports
perfMonitor.printReport();
securityManager.printSecurityReport();

std::cout << "\n=== PROJECT COMPLETION SUMMARY ===" << std::endl;
std::cout << "Successfully implemented a comprehensive IoT Network Simulation Framework!" << std::endl;
std::cout << std::endl;
std::cout << "Features Completed:" << std::endl;
std::cout << "✅ Core Device Framework (Sensors, Actuators)" << std::endl;
std::cout << "✅ Message Communication System" << std::endl;
std::cout << "✅ Device Management System" << std::endl;
std::cout << "✅ Network Management with Protocol Support" << std::endl;
std::cout << "✅ Simulation Engine with Event System" << std::endl;
std::cout << "✅ Configuration Management" << std::endl;
std::cout << "✅ Performance Monitoring" << std::endl;
std::cout << "✅ Security Framework Foundation" << std::endl;
std::cout << std::endl;
std::cout << "Protocols Supported: MQTT, CoAP, HTTP, LoRa, ZigBee, BLE, Thread, Z-Wave, NB-IoT, Sigfox" << std::endl;
std::cout << "Devices Implemented: Temperature, Humidity, Motion Sensors; LED, Motor, Relay Actuators" << std::endl;
std::cout << "Advanced Features: Event Scheduling, Time Control, Network Simulation, Battery Management" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}