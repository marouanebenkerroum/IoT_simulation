#ifndef IOT_SIMULATION_SIMULATION_ENGINE_H
#define IOT_SIMULATION_SIMULATION_ENGINE_H

#include "../core/DeviceManager.h"
#include "../network/NetworkManager.h"
#include <chrono>
#include <thread>
#include <functional>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <vector>

namespace iot{

    struct SimulationEvent{
        std::chrono::steady_clock::time_point scheduledTime;
        std::string eventId;
        std::function<void()> callback;
        int priority;

        bool operator>(const SimulationEvent& other) const {
            if (scheduledTime == other.scheduledTime) {
                return priority < other.priority;  // Higher priority first if same time
            }
            return scheduledTime > other.scheduledTime;  // Earlier time first
        }
    };

    struct SimulationConfig{
        double simulationSpeed;
        int maxDevices;
        double packetLossRate;
        double networkDelayMin;
        double networkDelayMax;
        std::string logLevel;
        std::string outputFile;
    };

    class SimulationEngine{
        public: 
            enum class State{
                STOPPED,
                RUNNING,
                PAUSED
            };
        private:
            
    };
}

#endif