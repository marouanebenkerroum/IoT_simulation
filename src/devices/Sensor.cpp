#include "../../include/devices/Sensor.h"
#include "../../include/core/Message.h"
#include <iostream>
#include <sstream>

namespace iot {
    
    Sensor::Sensor(const std::string& id, 
                   const std::string& name,
                   double minVal,
                   double maxVal)
        : IoTDevice(id, "Sensor", name)  // This properly initializes IoTDevice
        , currentValue(0.0)
        , minValue(minVal)
        , maxValue(maxVal)
        , rng(std::random_device{}())
        , noiseDistribution(-0.1, 0.1) {
    }
    
    void Sensor::sendData() {
        if (!isActive) return;
        
        currentValue = readValue();
        
        std::ostringstream oss;
        oss << currentValue;
        
        std::cout << "Sensor " << getDeviceId() << " sending data: " << currentValue << std::endl;
    }
    
    void Sensor::receiveData(const Message& message) {
        switch (message.getMessageType()) {
            case Message::MessageType::COMMAND:
                std::cout << "Sensor " << getDeviceId() << " received command: " 
                         << message.getPayload() << std::endl;
                if (message.getPayload() == "CALIBRATE") {
                    std::cout << "Calibrating sensor " << getDeviceId() << std::endl;
                } else if (message.getPayload() == "STATUS") {
                    std::cout << "Sensor status: " << getStatus() << std::endl;
                }
                break;
                
            case Message::MessageType::DATA:
                std::cout << "Sensor " << getDeviceId() << " received unexpected data message" << std::endl;
                break;
                
            case Message::MessageType::ERROR:
                std::cout << "Sensor " << getDeviceId() << " received error: " 
                         << message.getPayload() << std::endl;
                break;
                
            default:
                std::cout << "Sensor " << getDeviceId() << " received unknown message type" << std::endl;
                break;
        }
    }
    
} // namespace iot