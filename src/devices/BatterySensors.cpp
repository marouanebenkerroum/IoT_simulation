#include "../../include/devices/BatterySensors.h"
#include <iostream>
#include <thread>
#include <chrono>

namespace iot {
    
    // BatteryTemperatureSensor Implementation
    BatteryTemperatureSensor::BatteryTemperatureSensor(const std::string& id, const std::string& name)
        :Sensor(id, name, -40.0, 85.0)
        , battery()
        , baselineTemp(22.0)
        , rng(std::random_device{}())
        , noiseDistribution(-0.1, 0.1) {
        battery.setPowerConsumption(0.05);  // Low power consumption for temperature sensor
    }
    
    double BatteryTemperatureSensor::readValue() {
        // Simulate realistic temperature variations
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        std::tm* tm = std::localtime(&time_t);
        
        // Daily temperature cycle (cooler at night, warmer at day)
        double hourFactor = std::sin((tm->tm_hour - 6) * M_PI / 12.0) * 2.0;
        
        // Random noise
        double noise = noiseDistribution(rng) * 3.0;
        
        currentValue = baselineTemp + hourFactor + noise;
        currentValue = std::max(minValue, std::min(maxValue, currentValue));
        
        // Consume battery power for reading
        battery.consumePower(battery.getPowerConsumption() * 0.1);  // Very low power for reading
        
        return currentValue;
    }
    
    void BatteryTemperatureSensor::sendData() {
        if (!isActive || battery.getBatteryLevel() < 5.0) {
            std::cout << "BatteryTemperatureSensor " << getDeviceId() 
                     << " cannot send data (Battery: " << battery.getBatteryLevel() << "%)" << std::endl;
            return;
        }
        
        // Consume battery power for transmission
        battery.consumePower(battery.getPowerConsumption());
        
        std::cout << "BatteryTemperatureSensor " << getDeviceId() 
                 << " sending  " << currentValue << "°C (Battery: " 
                 << battery.getBatteryLevel() << "%)" << std::endl;
        
        Sensor::sendData();
    }
    
    void BatteryTemperatureSensor::receiveData(const Message& message) {
        // Handle commands like calibration, status queries
        Sensor::receiveData(message);
        // Consume small amount of power for processing
        battery.consumePower(battery.getPowerConsumption() * 0.05);
    }
    
    // BatteryMotionSensor Implementation
    BatteryMotionSensor::BatteryMotionSensor(const std::string& id, const std::string& name)
        : Sensor(id, name, 0.0, 1.0)
        , battery()
        , lastMotionState(false)
        , motionProbability(0.0, 1.0)
        , rng(std::random_device{}())
        , sleepInterval(30)   // 30 seconds sleep
        , activeDuration(5)   // 5 seconds active
    {
        battery.setPowerConsumption(0.2);  // Higher power consumption for motion detection
    }
    
    double BatteryMotionSensor::readValue() {
        if (battery.getBatteryLevel() < 5.0) {
            std::cout << "BatteryMotionSensor " << getDeviceId() 
                     << " battery too low to detect motion" << std::endl;
            return 0.0;
        }
        
        // Consume battery power for active sensing
        battery.consumePower(battery.getPowerConsumption() * 0.1);
        
        // Motion sensors return binary values (0 = no motion, 1 = motion detected)
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        std::tm* tm = std::localtime(&time_t);
        
        // Higher probability of motion during day hours
        double baseProbability = (tm->tm_hour >= 8 && tm->tm_hour <= 22) ? 0.15 : 0.05;
        
        // Add some randomness
        double randomValue = motionProbability(rng);
        
        currentValue = (randomValue < baseProbability) ? 1.0 : 0.0;
        
        return currentValue;
    }
    
    void BatteryMotionSensor::sendData() {
        if (!isActive || battery.getBatteryLevel() < 5.0) {
            std::cout << "BatteryMotionSensor " << getDeviceId() 
                     << " cannot send data (Battery: " << battery.getBatteryLevel() << "%)" << std::endl;
            return;
        }
        
        // Consume battery power for transmission
        battery.consumePower(battery.getPowerConsumption());
        
        std::cout << "BatteryMotionSensor " << getDeviceId() 
                 << " sending  " << (currentValue > 0.5 ? "MOTION" : "NO MOTION")
                 << " (Battery: " << battery.getBatteryLevel() << "%)" << std::endl;
        
        Sensor::sendData();
    }
    
    void BatteryMotionSensor::receiveData(const Message& message) {
        Sensor::receiveData(message);
        // Consume small amount of power for processing
        battery.consumePower(battery.getPowerConsumption() * 0.05);
    }
    
    void BatteryMotionSensor::setSleepPattern(int sleepSec, int activeSec) {
        sleepInterval = std::max(1, sleepSec);
        activeDuration = std::max(1, activeSec);
        std::cout << "BatteryMotionSensor " << getDeviceId() 
                 << " sleep pattern set: " << sleepInterval 
                 << "s sleep, " << activeDuration << "s active" << std::endl;
    }
    
} // namespace iot