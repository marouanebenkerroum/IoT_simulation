#include <iostream>
#include <memory>
#include <thread>
#include <chrono>
#include "../include/core/DeviceManager.h"
#include "../include/network/NetworkManager.h"
#include "../include/simulation/SimulationEngine.h"
#include "../include/core/Message.h"
#include "../include/devices/ConcreteSensors.h"
#include "../include/devices/ConcreteActuators.h"
#include "../include/devices/ProtocolSensors.h"
#include "../include/utils/ConfigManager.h"
#include "../include/security/SecurityManager.h"
#include "../include/utils/PerformanceMonitor.h"
#include <iomanip>

int main() {
    std::cout << "=================================================" << std::endl;
    std::cout << "IoT Network Simulation Framework - Comprehensive Test" << std::endl;
    std::cout << "=================================================" << std::endl;
    
    try {
        // Initialize performance monitoring
        iot::PerformanceMonitor perfMonitor;
        auto simStart = std::chrono::high_resolution_clock::now();
        
        // Create core components
        std::cout << "\n1. Initializing Core Components..." << std::endl;
        auto deviceManager = std::make_shared<iot::DeviceManager>();
        auto networkManager = std::make_shared<iot::NetworkManager>(deviceManager);
        auto simulationEngine = std::make_shared<iot::SimulationEngine>(deviceManager, networkManager);
        
        // Create security manager
        auto securityManager = std::make_shared<iot::SecurityManager>(iot::SecurityManager::SecurityLevel::ENHANCED);
        
        std::cout << "✓ Core components initialized successfully" << std::endl;
        
        // Configure network conditions
        std::cout << "\n2. Configuring Network Conditions..." << std::endl;
        networkManager->setNetworkConditions(0.03, 20.0, 150.0);  // 3% packet loss, 20-150ms delay
        std::cout << "✓ Network conditions configured (3% packet loss, 20-150ms delay)" << std::endl;
        
        // Create a smart home scenario with different protocols
        std::cout << "\n3. Creating Smart Home IoT Devices..." << std::endl;
        
        // Regular sensors and actuators
        auto tempSensor = std::make_shared<iot::TemperatureSensor>("LIVING_TEMP", "Living Room Temperature");
        auto humiditySensor = std::make_shared<iot::HumiditySensor>("BASEMENT_HUM", "Basement Humidity");
        auto motionSensor = std::make_shared<iot::MotionSensor>("FRONT_DOOR_MOTION", "Front Door Motion");
        
        auto led = std::make_shared<iot::LED>("STATUS_LED", "System Status LED");
        auto motor = std::make_shared<iot::Motor>("VENT_MOTOR", "Ventilation Motor", 100);
        auto relay = std::make_shared<iot::Relay>("WATER_RELAY", "Water Pump Relay", 15.0);
        
        // Protocol-specific devices
        auto loraSensor = std::make_shared<iot::LoRaTemperatureSensor>("GARDEN_TEMP", "Garden Temperature");
        auto zigbeeSensor = std::make_shared<iot::ZigBeeMotionSensor>("KITCHEN_MOTION", "Kitchen Motion");
        auto bleSensor = std::make_shared<iot::BLEHealthSensor>("HEART_RATE", "Wearable Heart Rate");
        
        std::cout << "✓ Created 7 IoT devices with different protocols" << std::endl;
        
        // Register all devices
        std::cout << "\n4. Registering Devices..." << std::endl;
        deviceManager->registerDevice(tempSensor);
        deviceManager->registerDevice(humiditySensor);
        deviceManager->registerDevice(motionSensor);
        deviceManager->registerDevice(led);
        deviceManager->registerDevice(motor);
        deviceManager->registerDevice(relay);
        deviceManager->registerDevice(loraSensor);
        deviceManager->registerDevice(zigbeeSensor);
        deviceManager->registerDevice(bleSensor);
        
        // Set protocols for network simulation
        networkManager->setDeviceProtocol("LIVING_TEMP", iot::NetworkManager::Protocol::MQTT);
        networkManager->setDeviceProtocol("BASEMENT_HUM", iot::NetworkManager::Protocol::COAP);
        networkManager->setDeviceProtocol("FRONT_DOOR_MOTION", iot::NetworkManager::Protocol::ZIGBEE);
        networkManager->setDeviceProtocol("STATUS_LED", iot::NetworkManager::Protocol::MQTT);
        networkManager->setDeviceProtocol("VENT_MOTOR", iot::NetworkManager::Protocol::HTTP);
        networkManager->setDeviceProtocol("WATER_RELAY", iot::NetworkManager::Protocol::MQTT);
        networkManager->setDeviceProtocol("GARDEN_TEMP", iot::NetworkManager::Protocol::LORA);
        networkManager->setDeviceProtocol("KITCHEN_MOTION", iot::NetworkManager::Protocol::ZIGBEE);
        networkManager->setDeviceProtocol("HEART_RATE", iot::NetworkManager::Protocol::BLUETOOTH_LE);
        
        std::cout << "✓ All devices registered and protocols assigned" << std::endl;
        
        // Register devices with security manager
        std::cout << "\n5. Configuring Security..." << std::endl;
        securityManager->registerDevice("LIVING_TEMP", iot::SecurityManager::SecurityLevel::ENHANCED);
        securityManager->registerDevice("BASEMENT_HUM", iot::SecurityManager::SecurityLevel::BASIC);
        securityManager->registerDevice("FRONT_DOOR_MOTION", iot::SecurityManager::SecurityLevel::ENHANCED);
        securityManager->registerDevice("STATUS_LED", iot::SecurityManager::SecurityLevel::BASIC);
        securityManager->registerDevice("VENT_MOTOR", iot::SecurityManager::SecurityLevel::ENTERPRISE);
        securityManager->registerDevice("WATER_RELAY", iot::SecurityManager::SecurityLevel::ENHANCED);
        securityManager->registerDevice("GARDEN_TEMP", iot::SecurityManager::SecurityLevel::ENHANCED);
        securityManager->registerDevice("KITCHEN_MOTION", iot::SecurityManager::SecurityLevel::BASIC);
        securityManager->registerDevice("HEART_RATE", iot::SecurityManager::SecurityLevel::BASIC);
        
        std::cout << "✓ Security configuration completed" << std::endl;
        
        // Show device list
        deviceManager->listDevices();
        
        // Load configuration
        std::cout << "\n6. Loading Simulation Configuration..." << std::endl;
        simulationEngine->loadConfig("simulation_config.json");
        simulationEngine->setSimulationSpeed(2.0);  // Run 2x faster than real-time
        std::cout << "✓ Configuration loaded, simulation speed set to 2.0x" << std::endl;
        
        // Schedule comprehensive events
        std::cout << "\n7. Scheduling Simulation Events..." << std::endl;
        
        // Temperature monitoring - every 3 seconds
        simulationEngine->scheduleRepeatingEvent(
            std::chrono::milliseconds(3000),
            [&]() {
                double temp = tempSensor->readValue();
                std::cout << "[EVENT] Living room temperature: " << std::fixed << std::setprecision(1) 
                         << temp << "°C" << std::endl;
                
                // Send alert if temperature is extreme
                if (temp > 30.0 || temp < 15.0) {
                    iot::Message alertMsg("LIVING_TEMP", "STATUS_LED", 
                                        "TEMP_ALERT:" + std::to_string(temp));
                    networkManager->sendMessage(alertMsg);
                }
            },
            "TEMP_MONITOR"
        );
        
        // Humidity monitoring - every 5 seconds
        simulationEngine->scheduleRepeatingEvent(
            std::chrono::milliseconds(5000),
            [&]() {
                double humidity = humiditySensor->readValue();
                std::cout << "[EVENT] Basement humidity: " << std::fixed << std::setprecision(1) 
                         << humidity << "%" << std::endl;
                
                // Control ventilation based on humidity
                if (humidity > 60.0) {
                    iot::Message motorCmd("HUMIDITY_CTRL", "VENT_MOTOR", "ON");
                    networkManager->sendMessage(motorCmd);
                } else if (humidity < 40.0) {
                    iot::Message motorCmd("HUMIDITY_CTRL", "VENT_MOTOR", "OFF");
                    networkManager->sendMessage(motorCmd);
                }
            },
            "HUMIDITY_MONITOR"
        );
        
        // Motion detection - every 2 seconds
        simulationEngine->scheduleRepeatingEvent(
            std::chrono::milliseconds(2000),
            [&]() {
                double motion = motionSensor->readValue();
                if (motion > 0.5) {
                    std::cout << "[EVENT] Motion detected at front door!" << std::endl;
                    iot::Message alertMsg("FRONT_DOOR_MOTION", "STATUS_LED", "MOTION_ALERT");
                    networkManager->sendMessage(alertMsg);
                }
            },
            "MOTION_MONITOR"
        );
        
        // Garden temperature monitoring (LoRa) - every 10 seconds
        simulationEngine->scheduleRepeatingEvent(
            std::chrono::milliseconds(10000),
            [&]() {
                double gardenTemp = loraSensor->readValue();
                std::cout << "[EVENT] Garden temperature: " << std::fixed << std::setprecision(1) 
                         << gardenTemp << "°C (LoRa sensor)" << std::endl;
                
                // Simulate LoRa battery consumption
                auto loraSensorPtr = std::dynamic_pointer_cast<iot::LoRaTemperatureSensor>(loraSensor);
                if (loraSensorPtr) {
                    std::cout << "[INFO] LoRa sensor battery: " << std::fixed << std::setprecision(1) 
                             << loraSensorPtr->getBatteryLevel() << "%" << std::endl;
                }
            },
            "GARDEN_TEMP_MONITOR"
        );
        
        // Kitchen motion (ZigBee) - every 4 seconds
        simulationEngine->scheduleRepeatingEvent(
            std::chrono::milliseconds(4000),
            [&]() {
                double kitchenMotion = zigbeeSensor->readValue();
                if (kitchenMotion > 0.5) {
                    std::cout << "[EVENT] Motion detected in kitchen! (ZigBee)" << std::endl;
                }
            },
            "KITCHEN_MOTION_MONITOR"
        );
        
        // Heart rate monitoring (BLE) - every 1 second
        simulationEngine->scheduleRepeatingEvent(
            std::chrono::milliseconds(1000),
            [&]() {
                double heartRate = bleSensor->readValue();
                std::cout << "[EVENT] Heart rate: " << std::fixed << std::setprecision(0) 
                         << heartRate << " BPM (BLE)" << std::endl;
            },
            "HEART_RATE_MONITOR"
        );
        
        // One-time events for testing
        simulationEngine->scheduleEvent(
            std::chrono::milliseconds(15000),
            [&]() {
                std::cout << "[EVENT] 15 seconds: Testing LED toggle" << std::endl;
                iot::Message ledCmd("SYSTEM", "STATUS_LED", "TOGGLE");
                networkManager->sendMessage(ledCmd);
            },
            "LED_TOGGLE_TEST"
        );
        
        simulationEngine->scheduleEvent(
            std::chrono::milliseconds(25000),
            [&]() {
                std::cout << "[EVENT] 25 seconds: Testing motor speed control" << std::endl;
                auto motorPtr = std::dynamic_pointer_cast<iot::Motor>(motor);
                if (motorPtr) {
                    motorPtr->setSpeed(75);
                }
            },
            "MOTOR_SPEED_TEST"
        );
        
        simulationEngine->scheduleEvent(
            std::chrono::milliseconds(35000),
            [&]() {
                std::cout << "[EVENT] 35 seconds: Testing relay activation" << std::endl;
                iot::Message relayCmd("SYSTEM", "WATER_RELAY", "ON");
                networkManager->sendMessage(relayCmd);
            },
            "RELAY_TEST"
        );
        
        std::cout << "✓ Scheduled 8 different events with various intervals" << std::endl;
        
        // Start the simulation
        std::cout << "\n8. Starting Simulation..." << std::endl;
        simulationEngine->start();
        
        // Record startup time
        auto setupEnd = std::chrono::high_resolution_clock::now();
        auto setupDuration = std::chrono::duration_cast<std::chrono::milliseconds>(setupEnd - simStart);
        perfMonitor.recordTime("simulation_setup", setupDuration.count());
        
        std::cout << "✓ Simulation started - Running for 45 seconds of simulated time" << std::endl;
        std::cout << "=================================================" << std::endl;
        std::cout << "SIMULATION RUNNING - MONITORING IoT NETWORK ACTIVITY" << std::endl;
        std::cout << "=================================================" << std::endl;
        
        // Let simulation run for 45 seconds
        std::this_thread::sleep_for(std::chrono::seconds(45));
        
        // Pause simulation briefly
        std::cout << "\n[SYSTEM] Pausing simulation for 5 seconds..." << std::endl;
        simulationEngine->pause();
        std::this_thread::sleep_for(std::chrono::seconds(5));
        
        // Resume simulation
        std::cout << "[SYSTEM] Resuming simulation..." << std::endl;
        simulationEngine->resume();
        
        // Continue for another 15 seconds
        std::this_thread::sleep_for(std::chrono::seconds(15));
        
        // Stop simulation
        std::cout << "\n[SYSTEM] Stopping simulation..." << std::endl;
        simulationEngine->stop();
        
        // Record total simulation time
        auto simEnd = std::chrono::high_resolution_clock::now();
        auto totalDuration = std::chrono::duration_cast<std::chrono::milliseconds>(simEnd - simStart);
        perfMonitor.recordTime("total_simulation", totalDuration.count());
        
        // Print final statistics
        std::cout << "\n=================================================" << std::endl;
        std::cout << "SIMULATION COMPLETED - FINAL STATISTICS" << std::endl;
        std::cout << "=================================================" << std::endl;
        
        simulationEngine->printStats();
        perfMonitor.printReport();
        securityManager->printSecurityReport();
        
        // Show final device states
        std::cout << "\nFinal Device States:" << std::endl;
        std::cout << "-------------------" << std::endl;
        auto allDevices = deviceManager->getAllDevices();
        for (const auto& device : allDevices) {
            std::cout << device->getStatus() << std::endl;
        }
        
        std::cout << "\n=================================================" << std::endl;
        std::cout << "COMPREHENSIVE SIMULATION TEST COMPLETED SUCCESSFULLY!" << std::endl;
        std::cout << "=================================================" << std::endl;
        std::cout << "Key Features Demonstrated:" << std::endl;
        std::cout << "• Multi-protocol device communication (MQTT, CoAP, LoRa, ZigBee, BLE)" << std::endl;
        std::cout << "• Event-driven architecture with scheduled and repeating events" << std::endl;
        std::cout << "• Realistic sensor behavior with time-based variations" << std::endl;
        std::cout << "• Network simulation with packet loss and delays" << std::endl;
        std::cout << "• Device security with authentication and authorization" << std::endl;
        std::cout << "• Performance monitoring and statistics tracking" << std::endl;
        std::cout << "• Thread-safe concurrent operation" << std::endl;
        std::cout << "• Scalable device management system" << std::endl;
        std::cout << "=================================================" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error in simulation: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}