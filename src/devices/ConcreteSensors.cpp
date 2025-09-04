#include "../../include/devices/ConcreteSensors.h"
#include <iostream>
#include <cmath>
#include <ctime>

namespace iot {
    
    // Temperature Sensor Implementation
    TemperatureSensor::TemperatureSensor(const std::string& id, const std::string& name)
        : Sensor(id, name, -40.0, 125.0)
        , baselineTemp(22.0) {
    }
    
    double TemperatureSensor::readValue() {
        // Simulate realistic temperature variations
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        std::tm* tm = std::localtime(&time_t);
        
        // Daily temperature cycle (cooler at night, warmer at day)
        double hourFactor = std::sin((tm->tm_hour - 6) * M_PI / 12.0) * 2.0;
        
        // Random noise
        double noise = noiseDistribution(rng) * 3.0;
        
        currentValue = baselineTemp + hourFactor + noise;
        
        // Clamp to valid range
        currentValue = std::max(minValue, std::min(maxValue, currentValue));
        
        return currentValue;
    }
    
    // Humidity Sensor Implementation  
    HumiditySensor::HumiditySensor(const std::string& id, const std::string& name)
        : Sensor(id, name, 0.0, 100.0)  // Properly initialize Sensor base class
        , baselineHumidity(45.0) {
    }
    
    double HumiditySensor::readValue() {
        // Simulate humidity variations
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        std::tm* tm = std::localtime(&time_t);
        
        // Inverse relationship with temperature
        double timeFactor = std::cos((tm->tm_hour - 6) * M_PI / 12.0) * 5.0;
        
        // Random noise
        double noise = noiseDistribution(rng) * 8.0;
        
        currentValue = baselineHumidity + timeFactor + noise;
        
        // Clamp to valid range (0-100%)
        currentValue = std::max(0.0, std::min(100.0, currentValue));
        
        return currentValue;
    }
    
    // Motion Sensor Implementation
    MotionSensor::MotionSensor(const std::string& id, const std::string& name)
        : Sensor(id, name, 0.0, 1.0)  // Properly initialize Sensor base class
        , lastMotionState(false)
        , motionProbability(0.0, 1.0) {
    }
    
    double MotionSensor::readValue() {
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
    
} // namespace iot