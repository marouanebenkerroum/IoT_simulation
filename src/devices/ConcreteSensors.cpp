#include "../../include/devices/ConcreteSensors.h"
#include <cmath>
#include <ctime>

namespace iot
{
    TemperatureSensor::TemperatureSensor(const std::string& id, const std::string& name)
    : Sensor(id, name, -40.0, 125.0)
    , baselineTemp(22.0){}

    double TemperatureSensor::readValue(){
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        std::tm* tm = std::localtime(&time_t);

        double hourFactor = std::sin((tm->tm_hour - 6) * M_PI / 12.0)*2.0;

        double noise = noiseDistribution(rng)*3.0;

        currentValue = baselineTemp + hourFactor + noise;

        currentValue = std::max(minValue, std::min(currentValue, maxValue));

        return currentValue;
    }

    HumiditySensor::HumiditySensor(const std::string& id, const std::string& name)
    : Sensor(id, name, 0.0, 100.0)
    , baselineHumidity(45.0){}

    double HumiditySensor::readValue(){
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        std::tm* tm = std::localtime(&time_t);
        
        double timeFactor = std::cos((tm->tm_hour - 6) * M_PI / 12.0)*5.0;
        
        double noise = noiseDistribution(rng)*8.0;
    
        currentValue = baselineHumidity + noise + timeFactor;

        currentValue = std::max(0.0, std::min(100.0, currentValue));

        return currentValue;
    }

    MotionSensor::MotionSensor(const std::string& id, const std::string& name)
    : Sensor(id, name, 0.0, 1.0)
    , lastMotionState(false)
    , motionProbability(0.0, 1.0){}

   double MotionSensor::readValue(){
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        std::tm* tm = std::localtime(&time_t);
        
        double baseProbability = (tm->tm_hour >= 8 && tm->tm_hour <= 22) ? 0.15 : 0.05;

        double randomValue = motionProbability(rng);

        currentValue = (randomValue < baseProbability) ? 1.0 : 0.0;
        return currentValue;
    }

    void MotionSensor::updateMotionPattern(){}
} // namespace iot
