#ifndef IOT_SIMULATION_PROTOCOL_AWARE_DEVICE_H
#define IOT_SIMULATION_PROTOCOL_AWARE_DEVICE_H

#include "../core/IoTDevice.h"
#include "../network/NetworkManager.h"
#include "../network/ProtocolCharacteristics.h"
#include <iostream>
namespace iot {
    
    class ProtocolAwareDevice {
    protected:
        NetworkManager::Protocol protocol;
        double batteryLevel;  // 0.0 to 100.0
        bool lowPowerMode;
        
    public:
        ProtocolAwareDevice(NetworkManager::Protocol proto = NetworkManager::Protocol::CUSTOM)
            : protocol(proto)
            , batteryLevel(100.0)
            , lowPowerMode(false) {
        }
        
        virtual ~ProtocolAwareDevice() = default;
        
        // Protocol-specific behavior
        virtual void enterLowPowerMode() {
            lowPowerMode = true;
            applyProtocolPowerSaving();
        }
        
        virtual void exitLowPowerMode() {
            lowPowerMode = false;
            wakeUpProtocolComponents();
        }
        
        // Battery management
        void consumeBattery(double amount) {
            batteryLevel = std::max(0.0, batteryLevel - amount);
            if (batteryLevel < 10.0 && !lowPowerMode) {
                enterLowPowerMode();
                std::cout << "Entering low power mode (Battery: " << batteryLevel << "%)" << std::endl;
            }
        }
        
        double getBatteryLevel() const { return batteryLevel; }
        NetworkManager::Protocol getProtocol() const { return protocol; }
        bool isInLowPowerMode() const { return lowPowerMode; }
        
        std::string getProtocolName() const {
            auto characteristics = getProtocolCharacteristics(protocol);
            return characteristics.name;
        }
        
    private:
        void applyProtocolPowerSaving() {
            switch (protocol) {
                case NetworkManager::Protocol::LORA:
                    std::cout << "LoRa power saving mode activated" << std::endl;
                    break;
                case NetworkManager::Protocol::ZIGBEE:
                    std::cout << "ZigBee power saving mode activated" << std::endl;
                    break;
                case NetworkManager::Protocol::BLUETOOTH_LE:
                    std::cout << "BLE power saving mode activated" << std::endl;
                    break;
                default:
                    std::cout << "Power saving mode activated" << std::endl;
                    break;
            }
        }
        
        void wakeUpProtocolComponents() {
            switch (protocol) {
                case NetworkManager::Protocol::LORA:
                    std::cout << "LoRa device waking up" << std::endl;
                    break;
                case NetworkManager::Protocol::ZIGBEE:
                    std::cout << "ZigBee device waking up" << std::endl;
                    break;
                default:
                    std::cout << "Device waking up" << std::endl;
                    break;
            }
        }
    };
    
} // namespace iot

#endif // IOT_SIMULATION_PROTOCOL_AWARE_DEVICE_H