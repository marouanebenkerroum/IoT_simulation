#include <iostream>
#include <memory>
#include "../include/core/IoTDevice.h"
#include "../include/core/Message.h"
#include "../include/devices/Sensor.h"
#include "../include/devices/Actuator.h"

namespace iot {
    class TemperatureSensor : public Sensor {
    public:
        TemperatureSensor(const std::string& id, const std::string& name)
            : Sensor(id, name, -40.0, 125.0) {}
        
        double readValue() override {
            // Simulate temperature reading around 22.5°C with some noise
            return 22.5 + noiseDistribution(rng) * 5.0;
        }
    };
    
    class LED : public Actuator {
    public:
        LED(const std::string& id, const std::string& name)
            : Actuator(id, name) {}
        
        void setState(bool newState) override {
            state = newState;
            std::cout << "LED " << getDeviceId() << " turned " 
                      << (state ? "ON" : "OFF") << std::endl;
        }
    };
}

int main() {
    std::cout << "IoT Network Simulation - Testing Message Handling" << std::endl;
    std::cout << "=================================================" << std::endl;
    
    try {
        // Create devices
        auto tempSensor = std::make_unique<iot::TemperatureSensor>("TEMP_001", "Living Room Temperature");
        auto led = std::make_unique<iot::LED>("LED_001", "Status LED");
        
        std::cout << "Created devices:" << std::endl;
        std::cout << tempSensor->getStatus() << std::endl;
        std::cout << led->getStatus() << std::endl << std::endl;
        
        // Test sending data
        std::cout << "Testing send data:" << std::endl;
        tempSensor->sendData();
        led->sendData();
        std::cout << std::endl;
        
        // Test receiving commands
        std::cout << "Testing receive commands:" << std::endl;
        
        // Send ON command to LED
        iot::Message onCommand("GATEWAY_01", "LED_001", "ON", iot::Message::MessageType::COMMAND);
        led->receiveData(onCommand);
        
        // Send OFF command to LED
        iot::Message offCommand("GATEWAY_01", "LED_001", "OFF", iot::Message::MessageType::COMMAND);
        led->receiveData(offCommand);
        
        // Send TOGGLE command to LED
        iot::Message toggleCommand("GATEWAY_01", "LED_001", "TOGGLE", iot::Message::MessageType::COMMAND);
        led->receiveData(toggleCommand);
        
        // Send STATUS command to sensor
        iot::Message statusCommand("GATEWAY_01", "TEMP_001", "STATUS", iot::Message::MessageType::COMMAND);
        tempSensor->receiveData(statusCommand);
        
        // Send CALIBRATE command to sensor
        iot::Message calibrateCommand("GATEWAY_01", "TEMP_001", "CALIBRATE", iot::Message::MessageType::COMMAND);
        tempSensor->receiveData(calibrateCommand);
        
        std::cout << std::endl << "Message handling tests completed successfully!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}