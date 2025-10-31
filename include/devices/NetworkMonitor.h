#ifndef IOT_SIMULATION_NETWORK_MONITOR_H
#define IOT_SIMULATION_NETWORK_MONITOR_H

#include "../core/IoTDevice.h"
#include "../core/Message.h"
#include <iostream>

namespace iot {

    class NetworkMonitor : public IoTDevice {
    public:
        NetworkMonitor(const std::string& id = "NETWORK_MONITOR",
                        const std::string& name = "Network Monitor")
            : IoTDevice(id, "MONITOR", name) {}

        void sendData() override {
            // Monitors do not proactively send data in this simulation
        }

        void receiveData(const Message& message) override {
            lastUpdate = std::chrono::steady_clock::now();
            isActive = true;
            std::cout << "[MONITOR] Received: " << message.toString() << std::endl;
        }
    };

} // namespace iot

#endif // IOT_SIMULATION_NETWORK_MONITOR_H


