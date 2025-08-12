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
            CUSTOM,         // Custom protocol
        LORA,           // Long Range Low Power WAN
        ZIGBEE,         // IEEE 802.15.4 mesh networking
        BLUETOOTH_LE,   // Bluetooth Low Energy
        THREAD,         // IPv6-based mesh networking
        ZWAVE,          // Wireless mesh protocol
        NB_IOT,         // Narrow Band IoT
        SIGFOX  
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
      
        explicit NetworkManager(std::shared_ptr<DeviceManager> dm);
     
        ~NetworkManager();
        
     
        void start();
  
        void stop();
 
        bool sendMessage(const Message& message);
        
        void broadcastMessage(const Message& message);
        

        void setDeviceProtocol(const std::string& deviceId, Protocol protocol);
        

        Protocol getDeviceProtocol(const std::string& deviceId) const;
        

        void setNetworkConditions(double packetLoss = 0.0, double delayMin = 0.0, double delayMax = 0.0);
        

        NetworkStats getStats() const;
        

        void resetStats();
        

        void printStats() const;
        
    private:

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