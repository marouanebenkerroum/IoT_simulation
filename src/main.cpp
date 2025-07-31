#include <iostream>
#include <memory>
#include <vector>
#include "../include/core/IoTDevice.h"
#include "../include/core/Message.h"
#include "../include/devices/ConcreteSensors.h"
#include "../include/devices/ConcreteActuators.h"

int main() {
    std::cout << "IoT Network Simulation - Day 2 Implementation" << std::endl;
    std::cout << "===============================================" << std::endl;
    
    try {
        // Create various sensor types
        auto tempSensor = std::make_unique<iot::TemperatureSensor>("TEMP_001", "Living Room Temperature");
        auto humiditySensor = std::make_unique<iot::HumiditySensor>("HUM_001", "Basement Humidity");
        auto motionSensor = std::make_unique<iot::MotionSensor>("MOTION_001", "Front Door Motion");
        
        // Create various actuator types
        auto led = std::make_unique<iot::LED>("LED_001", "Status LED");
        auto motor = std::make_unique<iot::Motor>("MOTOR_001", "Ventilation Motor", 100);
        auto relay = std::make_unique<iot::Relay>("RELAY_001", "Water Pump Relay", 15.0);
        
        std::cout << "\n=== Device Creation Test ===" << std::endl;
        std::cout << tempSensor->getStatus() << std::endl;
        std::cout << humiditySensor->getStatus() << std::endl;
        std::cout << motionSensor->getStatus() << std::endl;
        std::cout << led->getStatus() << std::endl;
        std::cout << motor->getStatus() << std::endl;
        std::cout << relay->getStatus() << std::endl;
        
        std::cout << "\n=== Sensor Reading Test ===" << std::endl;
        for (int i = 0; i < 5; i++) {
            std::cout << "Temperature: " << tempSensor->readValue() << "°C" << std::endl;
            std::cout << "Humidity: " << humiditySensor->readValue() << "%" << std::endl;
            std::cout << "Motion: " << (motionSensor->readValue() > 0.5 ? "DETECTED" : "CLEAR") << std::endl;
            std::cout << "---" << std::endl;
        }
        
        std::cout << "\n=== Actuator Control Test ===" << std::endl;
        
        // Test LED
        std::cout << "Testing LED:" << std::endl;
        led->setState(true);
        dynamic_cast<iot::LED*>(led.get())->setBrightness(128);
        dynamic_cast<iot::LED*>(led.get())->setColor("blue");
        
        // Test Motor
        std::cout << "\nTesting Motor:" << std::endl;
        motor->setState(true);
        dynamic_cast<iot::Motor*>(motor.get())->setSpeed(50);
        dynamic_cast<iot::Motor*>(motor.get())->setSpeed(-30);  // Reverse direction
        
        // Test Relay
        std::cout << "\nTesting Relay:" << std::endl;
        relay->setState(true);
        relay->setState(false);
        
        std::cout << "\n=== Message Handling Test ===" << std::endl;
        
        // Test receiving commands
        iot::Message ledOnMsg("GATEWAY", "LED_001", "ON", iot::Message::MessageType::COMMAND);
        led->receiveData(ledOnMsg);
        
        iot::Message motorSpeedMsg("GATEWAY", "MOTOR_001", "75", iot::Message::MessageType::COMMAND);
        motor->receiveData(motorSpeedMsg);
        
        std::cout << "\nDay 2 implementation completed successfully!" << std::endl;
        std::cout << "Successfully implemented:" << std::endl;
        std::cout << "- Temperature Sensor" << std::endl;
        std::cout << "- Humidity Sensor" << std::endl;
        std::cout << "- Motion Sensor" << std::endl;
        std::cout << "- LED Actuator" << std::endl;
        std::cout << "- Motor Actuator" << std::endl;
        std::cout << "- Relay Actuator" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}