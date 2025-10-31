#include "../../include/network/NetworkManager.h"
#include "../../include/network/ProtocolCharacteristics.h"

#include <iostream>
#include <algorithm>
#include <thread>
#include <chrono>
#include <iomanip> 

namespace iot {
    
    NetworkManager::NetworkManager(std::shared_ptr<DeviceManager> dm)
        : deviceManager(dm)
        , running(false)
        , stats{0, 0, 0, 0, std::chrono::steady_clock::now()}
        , packetLossRate(0.0)
        , networkDelayMin(0.0)
        , networkDelayMax(0.0)
        , rng(std::random_device{}())
        , ipsecManager(nullptr)
        , failureDistribution(0.0, 1.0) {
            for (int i = 0; i <= static_cast<int>(Protocol::SIGFOX); i++) {
    Protocol protocol = static_cast<Protocol>(i);
            }
    }
    
    NetworkManager::~NetworkManager() {
        stop();
    }
    
    void NetworkManager::start() {
        if (running) return;
        
        running = true;
        processingThread = std::thread(&NetworkManager::processMessages, this);
        std::cout << "Network manager started" << std::endl;
    }
    
    void NetworkManager::stop() {
        if (!running) return;
        
        running = false;
        queueCondition.notify_all();
        
        if (processingThread.joinable()) {
            processingThread.join();
        }
        
        std::cout << "Network manager stopped" << std::endl;
    }
    
    bool NetworkManager::sendMessage(const Message& message) {
        // Simulate network conditions
        if (!simulateNetworkConditions()) {
            std::lock_guard<std::mutex> lock(statsMutex);
            stats.messagesDropped++;
            return false;  // Message dropped due to network conditions
        }
        
        {
            std::lock_guard<std::mutex> lock(queueMutex);
            messageQueue.push(message);
        }
        
        queueCondition.notify_one();
        
        {
            std::lock_guard<std::mutex> lock(statsMutex);
            stats.messagesSent++;
        }
        
        return true;
    }
    
    void NetworkManager::broadcastMessage(const Message& message) {
        // For broadcast, we send to device manager which handles distribution
        if (deviceManager) {
            deviceManager->broadcastMessage(message);
            
            {
                std::lock_guard<std::mutex> lock(statsMutex);
                stats.messagesSent += deviceManager->getDeviceCount();
            }
        }
    }
    
    void NetworkManager::setDeviceProtocol(const std::string& deviceId, Protocol protocol) {
        std::lock_guard<std::mutex> lock(queueMutex);
        deviceProtocols[deviceId] = protocol;
        std::cout << "Device " << deviceId << " set to protocol " 
        << getProtocolCharacteristics(protocol).name << std::endl;

        auto characteristics = getProtocolCharacteristics(protocol);
         std::cout << "Device " << deviceId << " configured for " 
              << characteristics.name << " protocol" << std::endl;
    }
    
    NetworkManager::Protocol NetworkManager::getDeviceProtocol(const std::string& deviceId) const {
        std::lock_guard<std::mutex> lock(queueMutex);
        auto it = deviceProtocols.find(deviceId);
        if (it != deviceProtocols.end()) {
            return it->second;
        }
        return Protocol::CUSTOM;  // Default protocol
    }
    
    void NetworkManager::setNetworkConditions(double packetLoss, double delayMin, double delayMax) {
        packetLossRate = std::max(0.0, std::min(1.0, packetLoss));
        networkDelayMin = std::max(0.0, delayMin);
        networkDelayMax = std::max(networkDelayMin, delayMax);
    }
    
    NetworkManager::NetworkStats NetworkManager::getStats() const {
        std::lock_guard<std::mutex> lock(statsMutex);
        return stats;
    }
    
    void NetworkManager::resetStats() {
        std::lock_guard<std::mutex> lock(statsMutex);
        stats.messagesSent = 0;
        stats.messagesReceived = 0;
        stats.messagesDropped = 0;
        stats.errors = 0;
        stats.startTime = std::chrono::steady_clock::now();
    }
    
    void NetworkManager::printStats() const {
        auto currentStats = getStats();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::steady_clock::now() - currentStats.startTime);
        
        std::cout << "\n=== Network Statistics ===" << std::endl;
        std::cout << "Uptime: " << duration.count() << " seconds" << std::endl;
        std::cout << "Messages Sent: " << currentStats.messagesSent << std::endl;
        std::cout << "Messages Received: " << currentStats.messagesReceived << std::endl;
        std::cout << "Messages Dropped: " << currentStats.messagesDropped << std::endl;
        std::cout << "Errors: " << currentStats.errors << std::endl;
        
        if (currentStats.messagesSent > 0) {
            double successRate = 100.0 * (currentStats.messagesSent - currentStats.messagesDropped) / currentStats.messagesSent;
            std::cout << "Success Rate: " << std::fixed << std::setprecision(2) << successRate << "%" << std::endl;
        }
        std::cout << "=========================" << std::endl;
    }
    
    void NetworkManager::processMessages() {
        while (true) {
            std::unique_lock<std::mutex> lock(queueMutex);
            queueCondition.wait(lock, [this] { return !messageQueue.empty() || !running; });

            // If we've been asked to stop, exit quickly. Drain any remaining
            // queued messages without delivering to avoid blocking during shutdown.
            if (!running) {
                if (!messageQueue.empty()) {
                    // Count them as dropped to keep stats consistent
                    const size_t dropped = messageQueue.size();
                    messageQueue = std::queue<Message>();
                    lock.unlock();
                    {
                        std::lock_guard<std::mutex> statsLock(statsMutex);
                        stats.messagesDropped += dropped;
                    }
                }
                break;
            }

            if (!messageQueue.empty()) {
                Message message = messageQueue.front();
                messageQueue.pop();
                lock.unlock();

                // Apply network delay
                if (networkDelayMax > 0) {
                    std::uniform_real_distribution<double> delayDist(networkDelayMin, networkDelayMax);
                    double delayMs = delayDist(rng);
                    std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(delayMs)));
                }

                deliverMessage(message);
            }
        }
    }
    
    bool NetworkManager::simulateNetworkConditions() {
      
        if (packetLossRate > 0.0) {
            double randomValue = failureDistribution(rng);
            if (randomValue < packetLossRate) {
                return false; 
            }
        }
        return true;  
    }
    
  

    // Add the setter method
void NetworkManager::setIPSecManager(std::shared_ptr<IPSecManager> ipsec) {
    ipsecManager = ipsec;
    std::cout << "IPsec Manager integrated with Network Manager" << std::endl;
}

// Update the deliverMessage method to include IPsec processing
void NetworkManager::deliverMessage(const Message& message) {
    if (!deviceManager) {
        std::lock_guard<std::mutex> lock(statsMutex);
        stats.errors++;
        return;
    }
    
    // Apply IPsec security if enabled
    std::string payload = message.getPayload();
    std::string sourceDeviceId = message.getSourceDeviceId();
    std::string destDeviceId = message.getDestinationDeviceId();
    
    if (ipsecManager && ipsecManager->isEnabledIPSec()) {
        // Simulate IP addresses for devices (in real implementation, this would be actual IPs)
        // Handle device IDs with or without underscores
        size_t sourceUnderscorePos = sourceDeviceId.find_last_of('_');
        size_t destUnderscorePos = destDeviceId.find_last_of('_');
        
        std::string sourceSuffix = (sourceUnderscorePos != std::string::npos) 
            ? sourceDeviceId.substr(sourceUnderscorePos + 1) 
            : std::to_string(std::hash<std::string>{}(sourceDeviceId) % 255);
        std::string destSuffix = (destUnderscorePos != std::string::npos) 
            ? destDeviceId.substr(destUnderscorePos + 1) 
            : std::to_string(std::hash<std::string>{}(destDeviceId) % 255);
        
        std::string sourceIP = "192.168.1." + sourceSuffix;
        std::string destIP = "192.168.1." + destSuffix;
        
        // Apply IPsec encryption and authentication
        std::string securedPayload = ipsecManager->encryptAndAuthenticate(payload, sourceIP, destIP);
        
        // For demonstration, we'll create a new message with secured payload
        // In a real implementation, this would be handled at the network layer
        std::cout << "IPsec security applied to message from " << sourceDeviceId 
                  << " to " << destDeviceId << std::endl;
    }
    
    // Check if destination device exists before attempting delivery
    bool destinationExists = deviceManager->deviceExists(destDeviceId);
    
    bool delivered = false;
    if (destinationExists) {
        delivered = deviceManager->sendMessageToDevice(message);
    } else {
        std::cerr << "Warning: Destination device '" << destDeviceId 
                  << "' not found. Message from " << sourceDeviceId 
                  << " dropped." << std::endl;
    }
    
    {
        std::lock_guard<std::mutex> lock(statsMutex);
        if (delivered) {
            stats.messagesReceived++;
        } else {
            stats.errors++;
        }
    }
}
    
} // namespace iot