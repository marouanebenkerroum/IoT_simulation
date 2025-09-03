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
#include "../include/security/IPSecManager.h"
#include <iomanip>

int main() {
    std::cout << "=================================================" << std::endl;
    std::cout << "Enhanced IoT Network Simulation with IPsec Security" << std::endl;
    std::cout << "=================================================" << std::endl;
    
    try {
        // Create core components
        auto deviceManager = std::make_shared<iot::DeviceManager>();
        auto networkManager = std::make_shared<iot::NetworkManager>(deviceManager);
        auto simulationEngine = std::make_shared<iot::SimulationEngine>(deviceManager, networkManager);
        auto ipsecManager = std::make_shared<iot::IPSecManager>(iot::IPSecManager::IPsecMode::TRANSPORT);
        
        std::cout << "✓ Core components created" << std::endl;
        
        // Integrate IPsec with network manager
        networkManager->setIPSecManager(ipsecManager);
        std::cout << "✓ IPsec manager integrated" << std::endl;
        
        // Configure IPsec security policies
        std::cout << "\nConfiguring IPsec Security Policies..." << std::endl;
        
        // Create security associations for critical communications
        ipsecManager->createSecurityAssociation("192.168.1.1", "192.168.1.10");  // Temp sensor to LED
        ipsecManager->createSecurityAssociation("192.168.1.2", "192.168.1.20");  // Humidity to Motor
        ipsecManager->createSecurityAssociation("192.168.1.3", "192.168.1.30");  // Motion to Relay
        
        // Add security policies
        iot::SecurityPolicy criticalPolicy;
        criticalPolicy.sourceIP = "192.168.1.1";
        criticalPolicy.destinationIP = "192.168.1.10";
        criticalPolicy.protocol = "ESP";
        criticalPolicy.requireEncryption = true;
        criticalPolicy.requireAuthentication = true;
        criticalPolicy.securityLevel = 9;
        ipsecManager->addSecurityPolicy("192.168.1.1", "192.168.1.10", criticalPolicy);
        
        iot::SecurityPolicy normalPolicy;
        normalPolicy.sourceIP = "192.168.1.2";
        normalPolicy.destinationIP = "192.168.1.20";
        normalPolicy.protocol = "ESP";
        normalPolicy.requireEncryption = true;
        normalPolicy.requireAuthentication = true;
        normalPolicy.securityLevel = 7;
        ipsecManager->addSecurityPolicy("192.168.1.2", "192.168.1.20", normalPolicy);
        
        std::cout << "✓ IPsec security policies configured" << std::endl;
        
        // Create devices
        std::cout << "\nCreating IoT devices..." << std::endl;
        auto tempSensor = std::make_shared<iot::TemperatureSensor>("TEMP_SENSOR_1", "Living Room Temperature");
        auto humiditySensor = std::make_shared<iot::HumiditySensor>("HUM_SENSOR_2", "Basement Humidity");
        auto motionSensor = std::make_shared<iot::MotionSensor>("MOTION_SENSOR_3", "Front Door Motion");
        auto led = std::make_shared<iot::LED>("LED_ACTUATOR_10", "Status LED");
        auto motor = std::make_shared<iot::Motor>("MOTOR_ACTUATOR_20", "Ventilation Motor");
        auto relay = std::make_shared<iot::Relay>("RELAY_ACTUATOR_30", "Water Pump Relay");
        
        std::cout << "✓ Devices created" << std::endl;
        
        // Register devices
        std::cout << "\nRegistering devices..." << std::endl;
        deviceManager->registerDevice(tempSensor);
        deviceManager->registerDevice(humiditySensor);
        deviceManager->registerDevice(motionSensor);
        deviceManager->registerDevice(led);
        deviceManager->registerDevice(motor);
        deviceManager->registerDevice(relay);
        
        std::cout << "✓ Devices registered" << std::endl;
        
        // Configure network with moderate packet loss to test security resilience
        networkManager->setNetworkConditions(0.08, 15.0, 120.0);  // 8% packet loss, 15-120ms delay
        std::cout << "✓ Network configured with security testing conditions" << std::endl;
        
        // Show device list
        deviceManager->listDevices();
        
        // Configure simulation
        simulationEngine->setSimulationSpeed(1.2);  // Slightly faster than real-time
        std::cout << "✓ Simulation speed set to 1.2x" << std::endl;
        
        // Schedule security-aware events
        std::cout << "\nScheduling security-aware events..." << std::endl;
        
        // Temperature monitoring with IPsec protection
        simulationEngine->scheduleRepeatingEvent(
            std::chrono::milliseconds(2500),
            [&]() {
                double temp = tempSensor->readValue();
                std::cout << "[SENSOR] Temperature: " << std::fixed << std::setprecision(1) 
                         << temp << "°C" << std::endl;
                
                // Send temperature data with IPsec protection
                iot::Message tempMsg("TEMP_SENSOR_1", "LED_ACTUATOR_10", 
                                   "TEMP_ALERT:" + std::to_string(temp));
                networkManager->sendMessage(tempMsg);
            },
            "SECURE_TEMP_MONITORING"
        );
        
        // Humidity monitoring with IPsec protection
        simulationEngine->scheduleRepeatingEvent(
            std::chrono::milliseconds(3500),
            [&]() {
                double humidity = humiditySensor->readValue();
                std::cout << "[SENSOR] Humidity: " << std::fixed << std::setprecision(1) 
                         << humidity << "%" << std::endl;
                
                // Control ventilation based on humidity with security
                if (humidity > 60.0) {
                    iot::Message motorCmd("HUM_SENSOR_2", "MOTOR_ACTUATOR_20", "ON");
                    networkManager->sendMessage(motorCmd);
                }
            },
            "SECURE_HUMIDITY_CONTROL"
        );
        
        // Motion detection with IPsec protection
        simulationEngine->scheduleRepeatingEvent(
            std::chrono::milliseconds(4500),
            [&]() {
                double motion = motionSensor->readValue();
                if (motion > 0.5) {
                    std::cout << "[SENSOR] MOTION DETECTED - SECURE ALERT!" << std::endl;
                    iot::Message motionMsg("MOTION_SENSOR_3", "RELAY_ACTUATOR_30", "MOTION_ALERT");
                    networkManager->sendMessage(motionMsg);
                }
            },
            "SECURE_MOTION_DETECTION"
        );
        
        // Security monitoring events
        simulationEngine->scheduleRepeatingEvent(
            std::chrono::milliseconds(15000),
            [&]() {
                std::cout << "[SECURITY] IPsec Status Check" << std::endl;
                ipsecManager->printIPSecStatistics();
            },
            "IPSEC_MONITORING"
        );
        
        std::cout << "✓ Security-aware events scheduled" << std::endl;
        
        // Start simulation
        std::cout << "\nStarting enhanced simulation with IPsec security..." << std::endl;
        simulationEngine->start();
        
        std::cout << "=================================================" << std::endl;
        std::cout << "ENHANCED SIMULATION RUNNING - MONITORING WITH IPSEC" << std::endl;
        std::cout << "=================================================" << std::endl;
        
        // Run for 40 seconds to demonstrate IPsec in action
        std::this_thread::sleep_for(std::chrono::seconds(40));
        
        // Print final security statistics
        std::cout << "\n=================================================" << std::endl;
        std::cout << "FINAL IPSEC SECURITY STATISTICS" << std::endl;
        std::cout << "=================================================" << std::endl;
        ipsecManager->printIPSecStatistics();
        
        // Stop simulation
        std::cout << "\nStopping enhanced simulation..." << std::endl;
        simulationEngine->stop();
        
        std::cout << "\n=================================================" << std::endl;
        std::cout << "ENHANCED SIMULATION WITH IPSEC COMPLETED!" << std::endl;
        std::cout << "=================================================" << std::endl;
        std::cout << "Key Security Features Demonstrated:" << std::endl;
        std::cout << "• IPsec Security Association Management" << std::endl;
        std::cout << "• Security Policy Enforcement" << std::endl;
        std::cout << "• ESP Encryption and Authentication" << std::endl;
        std::cout << "• Secure Device Communication" << std::endl;
        std::cout << "• Network Resilience with Security" << std::endl;
        std::cout << "• Security Monitoring and Statistics" << std::endl;
        std::cout << "=================================================" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error in enhanced simulation: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}