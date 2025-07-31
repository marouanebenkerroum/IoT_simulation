#ifndef IOT_SIMULATION_NETWORK_MANAGER_H
#define IOT_SIMULATION_NETWORK_MANAGER_H

#include "../core/Message.h"
#include "../core/DeviceManager.h"
#include <queue>
#include <mutex>
#include <random>
#include <thread>
#include <condition_variable>
#include <chrono>
#include <map>

namespace iot {
    
    /**
     * @brief Network communication manager
     */
    class NetworkManager {
    public:
        enum class Protocol {
            MQTT,
            COAP,
            HTTP,
            CUSTOM
        };
        
        struct NetworkStats {
            size_t messagesSent;
            size_t messagesReceived;
            size_t messagesDropped;
            size_t errors;
            std::chrono::steady_clock::time_point startTime;
        };
        
    private:
        std::shared_ptr<DeviceManager> deviceManager;
        std::queue<Message> messageQueue;
        mutable std::mutex queueMutex;
        mutable std::mutex statsMutex;
        std::condition_variable queueCondition;
        bool running;
        std::thread processingThread;
        NetworkStats stats;
        std::map<std::string, Protocol> deviceProtocols;
        
        // Network failure simulation
        double packetLossRate;
        double networkDelayMin;
        double networkDelayMax;
        std::mt19937 rng;
        std::uniform_real_distribution<double> failureDistribution;
        
    public:
        /**
         * @brief Constructor
         */
        explicit NetworkManager(std::shared_ptr<DeviceManager> dm);
        
        /**
         * @brief Destructor
         */
        ~NetworkManager();
        
        /**
         * @brief Start network processing
         */
        void start();
        
        /**
         * @brief Stop network processing
         */
        void stop();
        
        /**
         * @brief Send message through network
         * @param message Message to send
         * @return true if message queued successfully
         */
        bool sendMessage(const Message& message);
        
        /**
         * @brief Broadcast message to all devices
         * @param message Message to broadcast
         */
        void broadcastMessage(const Message& message);
        
        /**
         * @brief Set protocol for device
         * @param deviceId Device ID
         * @param protocol Protocol to use
         */
        void setDeviceProtocol(const std::string& deviceId, Protocol protocol);
        
        /**
         * @brief Get protocol for device
         * @param deviceId Device ID
         * @return Protocol used by device
         */
        Protocol getDeviceProtocol(const std::string& deviceId) const;
        
        /**
         * @brief Set network failure parameters
         * @param packetLoss Packet loss rate (0.0 to 1.0)
         * @param delayMin Minimum delay in milliseconds
         * @param delayMax Maximum delay in milliseconds
         */
        void setNetworkConditions(double packetLoss = 0.0, double delayMin = 0.0, double delayMax = 0.0);
        
        /**
         * @brief Get network statistics
         * @return Network statistics
         */
        NetworkStats getStats() const;
        
        /**
         * @brief Reset network statistics
         */
        void resetStats();
        
        /**
         * @brief Print network statistics
         */
        void printStats() const;
        
    private:
        /**
         * @brief Process message queue
         */
        void processMessages();
        
        /**
         * @brief Simulate network delay and packet loss
         * @return true if message should be delivered, false if dropped
         */
        bool simulateNetworkConditions();
        
        /**
         * @brief Deliver message to destination
         * @param message Message to deliver
         */
        void deliverMessage(const Message& message);
    };
    
} // namespace iot

#endif // IOT_SIMULATION_NETWORK_MANAGER_H