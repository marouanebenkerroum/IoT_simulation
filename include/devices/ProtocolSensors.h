#ifndef IOT_SIMULATION_PROTOCOL_SENSORS_H
#define IOT_SIMULATION_PROTOCOL_SENSORS_H

#include "ProtocolAwareDevice.h"
#include "Sensor.h"
#include <iostream>
#include <random>

namespace iot {
    
    // LoRa Temperature Sensor - Optimized for long-range, low-power
    class LoRaTemperatureSensor : public Sensor, public ProtocolAwareDevice {
    private:
        int transmissionInterval;  // Seconds between transmissions
        bool dutyCycleLimit;       // Comply with LoRa duty cycle regulations
        double baselineTemp;
        std::mt19937 rng;
        std::uniform_real_distribution<double> noiseDistribution;
        
    public:
        LoRaTemperatureSensor(const std::string& id, const std::string& name)
            : Sensor(id, name, -40.0, 85.0)
            , ProtocolAwareDevice(NetworkManager::Protocol::LORA)
            , transmissionInterval(300)  // 5 minutes default
            , dutyCycleLimit(true)
            , baselineTemp(22.0)
            , rng(std::random_device{}())
            , noiseDistribution(-0.1, 0.1) {
        }
        
        double readValue() override {
            // Simulate realistic temperature variations for LoRa sensor
            double noise = noiseDistribution(rng) * 3.0;
            double currentValue = baselineTemp + noise;
            currentValue = std::max(minValue, std::min(maxValue, currentValue));
            
            // Simulate duty cycle compliance
            if (dutyCycleLimit) {
                consumeBattery(0.1);  // Very low power consumption for LoRa
            } else {
                consumeBattery(0.5);
            }
            
            return currentValue;
        }
        
        void sendData() override {
            if (getBatteryLevel() < 5.0) {
                std::cout << "LoRa sensor " << getDeviceId() << " battery too low to transmit" << std::endl;
                return;
            }
            
            std::cout << "LoRa sensor " << getDeviceId() << " transmitting data (Battery: " 
                     << getBatteryLevel() << "%)" << std::endl;
            Sensor::sendData();
            consumeBattery(1.0);  // Transmission power consumption
        }
        
        void setDutyCycleLimit(bool limit) { dutyCycleLimit = limit; }
        bool getDutyCycleLimit() const { return dutyCycleLimit; }
    };
    
    // ZigBee Motion Sensor - Optimized for mesh networking
    class ZigBeeMotionSensor : public Sensor, public ProtocolAwareDevice {
    private:
        bool meshRoutingEnabled;
        int hopCount;
        std::uniform_real_distribution<double> motionProbability;
        
    public:
        ZigBeeMotionSensor(const std::string& id, const std::string& name)
            : Sensor(id, name, 0.0, 1.0)
            , ProtocolAwareDevice(NetworkManager::Protocol::ZIGBEE)
            , meshRoutingEnabled(true)
            , hopCount(0)
            , motionProbability(0.0, 1.0) {
        }
        
        double readValue() override {
            // Motion sensors return binary values (0 = no motion, 1 = motion detected)
            double baseProbability = 0.15;  // Base motion probability
            double randomValue = motionProbability(rng);
            double currentValue = (randomValue < baseProbability) ? 1.0 : 0.0;
            
            // ZigBee sensors can route through mesh - consume more power
            consumeBattery(0.2);
            
            return currentValue;
        }
        
        void sendData() override {
            if (meshRoutingEnabled) {
                std::cout << "ZigBee sensor " << getDeviceId() << " using mesh routing (hops: " 
                         << hopCount << ", Battery: " << getBatteryLevel() << "%)" << std::endl;
                consumeBattery(0.5 + hopCount * 0.1);
            }
            Sensor::sendData();
        }
        
        void setHopCount(int hops) { hopCount = hops; }
        int getHopCount() const { return hopCount; }
        void setMeshRouting(bool enabled) { meshRoutingEnabled = enabled; }
    };
    
    // BLE Health Sensor - Optimized for wearable devices
    class BLEHealthSensor : public Sensor, public ProtocolAwareDevice {
    private:
        bool connectionOriented;
        int connectionInterval;  // ms
        double baselineValue;
        std::mt19937 rng;
        std::uniform_real_distribution<double> noiseDistribution;
        
    public:
        BLEHealthSensor(const std::string& id, const std::string& name)
            : Sensor(id, name, 0.0, 200.0)  // Heart rate range 0-200 BPM
            , ProtocolAwareDevice(NetworkManager::Protocol::BLUETOOTH_LE)
            , connectionOriented(true)
            , connectionInterval(7.5)  // 7.5ms default
            , baselineValue(72.0)  // Average resting heart rate
            , rng(std::random_device{}())
            , noiseDistribution(-0.05, 0.05) {
        }
        
        double readValue() override {
            // BLE sensors typically read frequently but transmit less
            double noise = noiseDistribution(rng) * 10.0;
            double currentValue = baselineValue + noise;
            currentValue = std::max(minValue, std::min(maxValue, currentValue));
            
            // BLE sensors typically read frequently but transmit less
            consumeBattery(0.05);  // Very low power for reading
            
            return currentValue;
        }
        
        void sendData() override {
            if (connectionOriented) {
                std::cout << "BLE sensor " << getDeviceId() << " sending via connection (Battery: " 
                         << getBatteryLevel() << "%)" << std::endl;
                consumeBattery(2.0);  // Higher power for connection maintenance
            }
            Sensor::sendData();
        }
        
        void setConnectionOriented(bool oriented) { connectionOriented = oriented; }
        bool isConnectionOriented() const { return connectionOriented; }
    };
    
} // namespace iot

#endif // IOT_SIMULATION_PROTOCOL_SENSORS_H