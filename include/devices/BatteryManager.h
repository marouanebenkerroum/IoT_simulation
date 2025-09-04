#ifndef IOT_SIMULATION_BATTERY_DEVICE_H
#define IOT_SIMULATION_BATTERY_DEVICE_H

#include <chrono>
#include <string>

namespace iot {
    
    /**
     * @brief Battery management for IoT devices
     */
    class BatteryManager {
    protected:
        double batteryLevel;  // 0-100%
        double powerConsumption;
        bool lowPowerMode;
        std::chrono::steady_clock::time_point lastUpdate;
        
    public:
        /**
         * @brief Constructor
         */
        BatteryManager();
        
        /**
         * @brief Virtual destructor
         */
        virtual ~BatteryManager() = default;
        
        /**
         * @brief Consume battery power
         * @param amount Power consumption amount (percentage)
         */
        void consumePower(double amount);
        
        /**
         * @brief Recharge battery
         * @param amount Recharge amount (percentage)
         */
        void rechargeBattery(double amount);
        
        /**
         * @brief Get current battery level
         * @return Battery level percentage (0-100)
         */
        double getBatteryLevel() const { return batteryLevel; }
        
        /**
         * @brief Check if battery is low
         * @return true if battery < 20%
         */
        bool isBatteryLow() const { return batteryLevel < 20.0; }
        
        /**
         * @brief Check if battery is critical
         * @return true if battery < 5%
         */
        bool isBatteryCritical() const { return batteryLevel < 5.0; }
        
        /**
         * @brief Get power consumption rate
         * @return Power consumption per operation
         */
        virtual double getPowerConsumption() const { return powerConsumption; }
        
        /**
         * @brief Set power consumption rate
         * @param consumption Power consumption per operation
         */
        void setPowerConsumption(double consumption) { powerConsumption = consumption; }
        
        /**
         * @brief Enter low power mode
         */
        virtual void enterLowPowerMode();
        
        /**
         * @brief Exit low power mode
         */
        virtual void exitLowPowerMode();
        
        /**
         * @brief Check if in low power mode
         * @return true if in low power mode
         */
        bool isInLowPowerMode() const { return lowPowerMode; }
    };
    
} // namespace iot

#endif // IOT_SIMULATION_BATTERY_DEVICE_H