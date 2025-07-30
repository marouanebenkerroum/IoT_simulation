#ifndef IOT_SIMULATION_CONCRETE_SENSORS_H
#define IOT_SIMULATION_CONCRETE_SENSORS_H

#include "Sensor.h"
#include <random>

namespace iot
{
    class TemperatureSensor : public Sensor{
        private: 
            double baselineTemp;

        public: 
            TemperatureSensor(const  std::string& id, const std::string& name);
            double readValue() override;
    };

    
    class HumiditySensor : public Sensor {
    private:
        double baselineHumidity;
        
    public:
        HumiditySensor(const std::string& id, const std::string& name);
        double readValue() override;
    };


    class MotionSensor : public Sensor {
    private:
        bool lastMotionState;
        std::uniform_real_distribution<double> motionProbability;
        
    public:
        MotionSensor(const std::string& id, const std::string& name);
        double readValue() override;
        void updateMotionPattern();
    };

} // namespace iot

#endif