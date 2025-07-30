#include "../../include/devices/Actuator.h"
#include "../../include/core/Message.h"
#include <iostream>
#include <algorithm>

namespace iot {
    
    Actuator::Actuator(const std::string& id, const std::string& name)
        : IoTDevice(id, "Actuator", name)
        , state(false) {
    }
    
    void Actuator::sendData() {
        if (!isActive) return;
        
        // Send current status
        std::cout << "Actuator " << deviceId << " status: " 
                 << (state ? "ON" : "OFF") << std::endl;
    }
    
    void Actuator::receiveData(const Message& message) {
        // Actuators primarily receive commands to control their state
        switch (message.getMessageType()) {
            case Message::MessageType::COMMAND:{
                std::cout << "Actuator " << deviceId << " received command: " 
                         << message.getPayload() << std::endl;
                
                // Process commands
                std::string command = message.getPayload();
                std::transform(command.begin(), command.end(), command.begin(), ::toupper);
                
                if (command == "ON" || command == "1" || command == "TRUE") {
                    setState(true);
                } else if (command == "OFF" || command == "0" || command == "FALSE") {
                    setState(false);
                } else if (command == "TOGGLE") {
                    toggle();
                } else if (command == "STATUS") {
                    std::cout << "Actuator " << deviceId << " current state: " 
                             << (state ? "ON" : "OFF") << std::endl;
                } else {
                    std::cout << "Actuator " << deviceId << " unknown command: " 
                             << command << std::endl;
                }
                break;
            }
            case Message::MessageType::DATA:
                std::cout << "Actuator " << deviceId << " received data: " 
                         << message.getPayload() << std::endl;
                // Might receive configuration data or setpoints
                break;
                
            case Message::MessageType::ERROR:
                std::cout << "Actuator " << deviceId << " received error: " 
                         << message.getPayload() << std::endl;
                break;
                
            default:
                std::cout << "Actuator " << deviceId << " received unknown message type" << std::endl;
                break;
        }
    }
    
    void Actuator::setState(bool newState) {
        state = newState;
        std::cout << "Actuator " << deviceId << " set to " 
                 << (state ? "ON" : "OFF") << std::endl;
    }
    
    void Actuator::toggle() {
        state = !state;
        std::cout << "Actuator " << deviceId << " toggled to " 
                 << (state ? "ON" : "OFF") << std::endl;
    }
    
} // namespace iot