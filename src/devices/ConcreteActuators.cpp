#include "../../include/devices/ConcreteActuators.h"
#include <iostream>
#include <algorithm>
#include <random>
namespace iot {
    
    // LED Implementation
    LED::LED(const std::string& id, const std::string& name)
        : Actuator(id, name)
        , brightness(0)
        , color("white") {
    }
    
    void LED::setState(bool newState) {
        state = newState;
        brightness = newState ? 255 : 0;  // Full brightness when on
        std::cout << "LED " << deviceId << " turned " 
                  << (state ? "ON" : "OFF") 
                  << " (Brightness: " << brightness 
                  << ", Color: " << color << ")" << std::endl;
    }
    
    void LED::setBrightness(int level) {
        brightness = std::max(0, std::min(255, level));
        if (brightness > 0) {
            state = true;
        } else {
            state = false;
        }
        std::cout << "LED " << deviceId << " brightness set to " << brightness << std::endl;
    }
    
    void LED::setColor(const std::string& newColor) {
        color = newColor;
        std::cout << "LED " << deviceId << " color changed to " << color << std::endl;
    }
    
    // Motor Implementation
    Motor::Motor(const std::string& id, const std::string& name, int maxSpd)
        : Actuator(id, name)
        , speed(0)
        , maxSpeed(std::abs(maxSpd)) {
    }
    
    void Motor::setState(bool newState) {
        state = newState;
        speed = newState ? maxSpeed : 0;
        std::cout << "Motor " << deviceId << " turned " 
                  << (state ? "ON" : "OFF") 
                  << " (Speed: " << speed << ")" << std::endl;
    }
    
    void Motor::setSpeed(int newSpeed) {
        speed = std::max(-maxSpeed, std::min(maxSpeed, newSpeed));
        state = (speed != 0);
        std::cout << "Motor " << deviceId << " speed set to " << speed << std::endl;
    }
    
    void Motor::stop() {
        speed = 0;
        state = false;
        std::cout << "Motor " << deviceId << " stopped" << std::endl;
    }
    
    // Relay Implementation
    Relay::Relay(const std::string& id, const std::string& name, double maxCurr)
        : Actuator(id, name)
        , current(0.0)
        , maxCurrent(maxCurr)
        , overloadProtection(true) {
    }
    
    void Relay::setState(bool newState) {
        if (newState && overloadProtection && isOverloaded()) {
            std::cout << "Relay " << deviceId << " OVERLOAD PROTECTION - Cannot turn ON!" << std::endl;
            return;
        }
        
        state = newState;
        current = newState ? (maxCurrent * 0.8) : 0.0;  // Simulate 80% load when active
        std::cout << "Relay " << deviceId << " turned " 
                  << (state ? "ON" : "OFF") 
                  << " (Current: " << current << "A)" << std::endl;
    }
    
    bool Relay::isOverloaded() const {
        // Simulate random overload conditions
        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::uniform_real_distribution<> dis(0.0, 1.0);
        
        return (dis(gen) < 0.05);  // 5% chance of overload
    }
    
} // namespace iot