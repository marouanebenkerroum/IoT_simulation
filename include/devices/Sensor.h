#ifndef IOT_SIMULATION_SENSOR_H
#define IOT_SIMULATION_SENSOR_H


#include "../core/IoTDevice.h"
#include <random>

namespace iot{

    class Sensor : public IoTDevice{
        protected:
            double currentValue;
            double minValue;
            double maxValue;
            std::mt19937 rng;
            std::uniform_real_distribution<double> noiseDistribution;
        
        public: 
            Sensor(const std::string& id,
                    const std::string& name,
                    double minVal = 0.0,
                    double maxVal = 100.0);
            
            virtual ~Sensor() = default;

            virtual double readValue() = 0;

            void sendData() override;

            void receiveData(const Message& message) override;

            double getCurrentValue() const { return currentValue; }
        
            double getMinValue() const { return minValue; }
        
            double getMaxValue() const { return maxValue; }
    };
}
#endif