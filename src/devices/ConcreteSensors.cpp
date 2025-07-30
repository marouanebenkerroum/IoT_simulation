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
    }
} // namespace iot
