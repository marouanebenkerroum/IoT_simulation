#include "../../include/network/NetworkManager.h"
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
        , failureDistribution(0.0, 1.0) {
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
        while (running) {
            std::unique_lock<std::mutex> lock(queueMutex);
            queueCondition.wait(lock, [this] { return !messageQueue.empty() || !running; });
            
            if (!running && messageQueue.empty()) {
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
    
    void NetworkManager::deliverMessage(const Message& message) {
        if (!deviceManager) {
            std::lock_guard<std::mutex> lock(statsMutex);
            stats.errors++;
            return;
        }
        
        bool delivered = deviceManager->sendMessageToDevice(message);
        
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