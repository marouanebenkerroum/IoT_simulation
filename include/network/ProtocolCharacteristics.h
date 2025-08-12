#ifndef IOT_SIMULATION_PROTOCOL_CHARACTERISTICS_H
#define IOT_SIMULATION_PROTOCOL_CHARACTERISTICS_H

#include "NetworkManager.h"
#include <string>

namespace iot {
    
    struct ProtocolCharacteristics {
        std::string name;
        double maxRangeKm;          // Maximum communication range
        double dataRateKbps;        // Typical data rate
        double powerConsumption;    // Relative power consumption (1.0 = baseline)
        double latencyMs;           // Typical latency
        size_t maxPayloadBytes;     // Maximum payload size
        bool supportsMesh;          // Mesh networking capability
        bool supportsEncryption;    // Built-in encryption support
        double typicalPacketLoss;   // Typical packet loss rate
        int maxDevicesPerNetwork;   // Maximum devices per network
        std::string typicalUseCase; // Common applications
    };
    
    // Factory function to get protocol characteristics
    inline ProtocolCharacteristics getProtocolCharacteristics(NetworkManager::Protocol protocol) {
        switch (protocol) {
            case NetworkManager::Protocol::LORA:
                return {"LoRa", 15.0, 0.3, 0.1, 1000.0, 256, false, true, 0.02, 1000, "Long-range sensors"};
            case NetworkManager::Protocol::ZIGBEE:
                return {"ZigBee", 0.1, 250.0, 0.3, 30.0, 100, true, true, 0.01, 65000, "Home automation"};
            case NetworkManager::Protocol::BLUETOOTH_LE:
                return {"Bluetooth LE", 0.05, 1000.0, 0.2, 10.0, 255, false, true, 0.05, 20, "Wearable devices"};
            case NetworkManager::Protocol::THREAD:
                return {"Thread", 0.05, 250.0, 0.4, 20.0, 1280, true, true, 0.01, 250, "Smart home"};
            case NetworkManager::Protocol::ZWAVE:
                return {"Z-Wave", 0.05, 100.0, 0.3, 50.0, 64, true, true, 0.01, 232, "Home security"};
            case NetworkManager::Protocol::NB_IOT:
                return {"NB-IoT", 10.0, 250.0, 0.15, 2000.0, 1600, false, true, 0.03, 50000, "Smart metering"};
            case NetworkManager::Protocol::SIGFOX:
                return {"Sigfox", 50.0, 0.01, 0.05, 5000.0, 12, false, true, 0.05, 1000000, "Low-power sensors"};
            case NetworkManager::Protocol::MQTT:
                return {"MQTT", 0.01, 10000.0, 1.0, 5.0, 268435456, false, false, 0.001, 1000000, "Enterprise IoT"};
            case NetworkManager::Protocol::COAP:
                return {"CoAP", 0.01, 1000.0, 0.8, 100.0, 1024, false, false, 0.005, 10000, "Constrained devices"};
            case NetworkManager::Protocol::HTTP:
                return {"HTTP", 0.01, 10000.0, 1.0, 50.0, 268435456, false, false, 0.001, 1000000, "Web services"};
            default:
                return {"Custom", 1.0, 1000.0, 1.0, 100.0, 1024, false, false, 0.01, 1000, "General purpose"};
        }
    }
    
} 

#endif 