#include "../../include/devices/Sensor.h"
#include "../../include/core/Message.h"
#include <iostream>
#include <sstream>

namespace iot {
    
    Sensor::Sensor(const std::string& id, 
                   const std::string& name,
                   double minVal,
                   double maxVal)
        : IoTDevice(id, "Sensor", name)
        , currentValue(0.0)
        , minValue(minVal)
        , maxValue(maxVal)
        , rng(std::random_device{}())
        , noiseDistribution(-0.1, 0.1) {  // ±10% noise
    }
    
    void Sensor::sendData() {
        if (!isActive) return;
        
        // Read current value
        currentValue = readValue();
        
        // Create message with sensor data
        std::ostringstream oss;
        oss << currentValue;
        
        // In a real implementation, this would send to network manager
        std::cout << "Sensor " << deviceId << " sending data: " << currentValue << std::endl;
    }
    
    void Sensor::receiveData(const Message& message) {
        // Sensors typically don't receive data, but they might receive:
        // - Configuration commands
        // - Calibration requests
        // - Status queries
        
        switch (message.getMessageType()) {
            case Message::MessageType::COMMAND:
                std::cout << "Sensor " << deviceId << " received command: " 
                         << message.getPayload() << std::endl;
                // Handle specific commands
                if (message.getPayload() == "CALIBRATE") {
                    std::cout << "Calibrating sensor " << deviceId << std::endl;
                } else if (message.getPayload() == "STATUS") {
                    std::cout << "Sensor status: " << getStatus() << std::endl;
                }
                break;
                
            case Message::MessageType::DATA:
                std::cout << "Sensor " << deviceId << " received unexpected data message" << std::endl;
                break;
                
            case Message::MessageType::ERROR:
                std::cout << "Sensor " << deviceId << " received error: " 
                         << message.getPayload() << std::endl;
                break;
                
            default:
                std::cout << "Sensor " << deviceId << " received unknown message type" << std::endl;
                break;
        }
    }
    
} // namespace iot