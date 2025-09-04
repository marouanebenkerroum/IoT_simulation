#include "../../include/devices/BatteryManager.h"
#include <algorithm>
#include <iostream>

namespace iot {
    
    BatteryManager::BatteryManager()
        : batteryLevel(100.0)
        , powerConsumption(0.1)  // Default power consumption
        , lowPowerMode(false)
        , lastUpdate(std::chrono::steady_clock::now()) {
    }
    
    void BatteryManager::consumePower(double amount) {
        batteryLevel = std::max(0.0, batteryLevel - amount);
        lastUpdate = std::chrono::steady_clock::now();
        
        if (batteryLevel < 5.0 && !lowPowerMode) {
            enterLowPowerMode();
            std::cout << "Battery entering low power mode (Level: " 
                     << batteryLevel << "%)" << std::endl;
        } else if (batteryLevel < 20.0) {
            std::cout << "Battery low: " << batteryLevel << "%" << std::endl;
        }
    }
    
    void BatteryManager::rechargeBattery(double amount) {
        batteryLevel = std::min(100.0, batteryLevel + amount);
        lastUpdate = std::chrono::steady_clock::now();
        
        if (lowPowerMode && batteryLevel > 20.0) {
            exitLowPowerMode();
            std::cout << "Battery exiting low power mode (Level: " 
                     << batteryLevel << "%)" << std::endl;
        }
    }
    
    void BatteryManager::enterLowPowerMode() {
        if (!lowPowerMode) {
            lowPowerMode = true;
            std::cout << "Entered low power mode" << std::endl;
        }
    }
    
    void BatteryManager::exitLowPowerMode() {
        if (lowPowerMode) {
            lowPowerMode = false;
            std::cout << "Exited low power mode" << std::endl;
        }
    }
    
} // namespace iot