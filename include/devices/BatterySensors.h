#ifndef IOT_SIMULATION_BATTERY_SENSORS_H
#define IOT_SIMULATION_BATTERY_SENSORS_H

#include "Sensor.h"
#include "BatteryManager.h"
#include <random>

namespace iot {
    
    /**
     * @brief Battery-powered Temperature Sensor
     */
    class BatteryTemperatureSensor : public Sensor {
    private:
        BatteryManager battery;
        double baselineTemp;
        std::mt19937 rng;
        std::uniform_real_distribution<double> noiseDistribution;
        
    public:
        BatteryTemperatureSensor(const std::string& id, const std::string& name);
        double readValue() override;
        void sendData() override;
        void receiveData(const Message& message) override;
        
        // Battery access methods
        double getBatteryLevel() const { return battery.getBatteryLevel(); }
        bool isBatteryLow() const { return battery.isBatteryLow(); }
        bool isBatteryCritical() const { return battery.isBatteryCritical(); }
        bool isInLowPowerMode() const { return battery.isInLowPowerMode(); }
        void rechargeBattery(double amount) { battery.rechargeBattery(amount); }
    };
    
    /**
     * @brief Battery-powered Motion Sensor with sleep cycles
     */
    class BatteryMotionSensor : public Sensor {
    private:
        BatteryManager battery;
        bool lastMotionState;
        std::uniform_real_distribution<double> motionProbability;
        std::mt19937 rng;
        int sleepInterval;  // Seconds between active periods
        int activeDuration; // Seconds of active sensing per cycle
        
    public:
        BatteryMotionSensor(const std::string& id, const std::string& name);
        double readValue() override;
        void sendData() override;
        void receiveData(const Message& message) override;
        void setSleepPattern(int sleepSec, int activeSec);
        
        // Battery access methods
        double getBatteryLevel() const { return battery.getBatteryLevel(); }
        bool isBatteryLow() const { return battery.isBatteryLow(); }
        bool isBatteryCritical() const { return battery.isBatteryCritical(); }
        bool isInLowPowerMode() const { return battery.isInLowPowerMode(); }
        void rechargeBattery(double amount) { battery.rechargeBattery(amount); }
    };
    
} // namespace iot

#endif // IOT_SIMULATION_BATTERY_SENSORS_H