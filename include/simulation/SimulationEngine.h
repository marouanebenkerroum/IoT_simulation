#ifndef IOT_SIMULATION_SIMULATION_ENGINE_H
#define IOT_SIMULATION_SIMULATION_ENGINE_H

#include "../core/DeviceManager.h"
#include "../network/NetworkManager.h"
#include "../utils/ConfigManager.h"  
#include <chrono>
#include <thread>
#include <functional>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <vector>

namespace iot {
    
    /**
     * @brief Event structure for scheduled simulation events
     */
    struct SimulationEvent {
        std::chrono::steady_clock::time_point scheduledTime;
        std::string eventId;
        std::function<void()> callback;
        int priority;
        
        // Comparison operator for priority queue (earliest time first)
        bool operator>(const SimulationEvent& other) const {
            if (scheduledTime == other.scheduledTime) {
                return priority < other.priority;  // Higher priority first if same time
            }
            return scheduledTime > other.scheduledTime;  // Earlier time first
        }
    };
    
    /**
     * @brief Simulation configuration structure
     */
    struct SimulationConfig {
        double simulationSpeed;  // 1.0 = real-time, >1.0 = faster, <1.0 = slower
        int maxDevices;
        double packetLossRate;
        double networkDelayMin;
        double networkDelayMax;
        std::string logLevel;
        std::string outputFile;
    };
    
    /**
     * @brief Main simulation engine
     */
    class SimulationEngine {
    public:
        enum class State {
            STOPPED,
            RUNNING,
            PAUSED
        };
        
    private:
        std::shared_ptr<DeviceManager> deviceManager;
        std::shared_ptr<NetworkManager> networkManager;
        
        State currentState;
        std::chrono::steady_clock::time_point startTime;
        std::chrono::steady_clock::time_point currentTime;
        std::chrono::milliseconds simulationTimeStep;
        double simulationSpeed;
        
        // Event system
        std::priority_queue<SimulationEvent, std::vector<SimulationEvent>, std::greater<SimulationEvent>> eventQueue;
        mutable std::mutex eventMutex;
        std::condition_variable eventCondition;
        
        // Threading
        bool running;
        std::thread simulationThread;
        mutable std::mutex stateMutex;
        
        // Configuration
        SimulationConfig config;
        
        // Statistics
        size_t totalEventsProcessed;
        size_t simulationSteps;
        
    public:
        /**
         * @brief Constructor
         */
        SimulationEngine(std::shared_ptr<DeviceManager> dm, std::shared_ptr<NetworkManager> nm);
        
        /**
         * @brief Destructor
         */
        ~SimulationEngine();
        
        /**
         * @brief Start the simulation
         */
        void start();
        
        /**
         * @brief Stop the simulation
         */
        void stop();
        
        /**
         * @brief Pause the simulation
         */
        void pause();
        
        /**
         * @brief Resume the simulation
         */
        void resume();
        
        /**
         * @brief Get current simulation state
         */
        State getState() const;
        
        /**
         * @brief Schedule an event
         */
        void scheduleEvent(const std::chrono::milliseconds& delay, 
                          std::function<void()> callback,
                          const std::string& eventId = "",
                          int priority = 0);
        
        /**
         * @brief Schedule a repeating event
         */
        void scheduleRepeatingEvent(const std::chrono::milliseconds& interval,
                                   std::function<void()> callback,
                                   const std::string& eventId = "",
                                   int priority = 0);
        
        /**
         * @brief Set simulation speed
         */
        void setSimulationSpeed(double speed);
        
        /**
         * @brief Get current simulation time
         */
        std::chrono::steady_clock::time_point getCurrentTime() const;
        
        /**
         * @brief Load configuration from file
         */
        bool loadConfig(const std::string& configFile);
        
        /**
         * @brief Get simulation statistics
         */
        void printStats() const;
        
    private:
        /**
         * @brief Main simulation loop
         */
        void runSimulation();
        
        /**
         * @brief Process pending events
         */
        void processEvents();
        
        /**
         * @brief Execute a single simulation step
         */
        void simulationStep();
    };
    
} // namespace iot

#endif // IOT_SIMULATION_SIMULATION_ENGINE_H