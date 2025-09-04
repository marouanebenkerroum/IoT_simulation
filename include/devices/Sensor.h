#ifndef IOT_SIMULATION_SENSOR_H
#define IOT_SIMULATION_SENSOR_H

#include "../core/IoTDevice.h"
#include "../core/Message.h"
#include <random>

namespace iot {
    
    class Sensor : public IoTDevice {
    protected:
        double currentValue;
        double minValue;
        double maxValue;
        std::mt19937 rng;
        std::uniform_real_distribution<double> noiseDistribution;
        
    public:
        // Make sure this constructor is PUBLIC and properly defined
        Sensor(const std::string& id, 
               const std::string& name,
               double minVal = 0.0,
               double maxVal = 100.0);
        
        virtual ~Sensor() = default;
        virtual double readValue() = 0;
        virtual void sendData();
        virtual void receiveData(const Message& message);
        
        // Getters
        double getCurrentValue() const { return currentValue; }
        double getMinValue() const { return minValue; }
        double getMaxValue() const { return maxValue; }
    };
    
} // namespace iot

#endif // IOT_SIMULATION_SENSOR_H