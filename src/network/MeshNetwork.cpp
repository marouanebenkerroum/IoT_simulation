#include "../../include/network/MeshNetwork.h"
#include <iostream>
#include <queue>
#include <algorithm>

namespace iot {
    
    MeshNetwork::MeshNetwork(int maxHopCount)
        : maxHops(maxHopCount)
        , gatewayId("") {
    }
    
    bool MeshNetwork::addDevice(const std::string& deviceId, bool isGatewayNode) {
        if (nodes.find(deviceId) != nodes.end()) {
            std::cout << "Device " << deviceId << " already exists in mesh network" << std::endl;
            return false;
        }
        
        MeshNode node;
        node.deviceId = deviceId;
        node.hopCountToGateway = maxHops;  // Initialize to max (unreachable)
        node.isGateway = isGatewayNode;
        node.signalStrength = 100.0;  // Default signal strength
        
        nodes[deviceId] = node;
        
        if (isGatewayNode) {
            gatewayId = deviceId;
            nodes[deviceId].hopCountToGateway = 0;  // Gateway has 0 hops to itself
        }
        
        std::cout << "Device " << deviceId << " added to mesh network" 
                 << (isGatewayNode ? " (GATEWAY)" : "") << std::endl;
        return true;
    }
    
    bool MeshNetwork::addNeighbor(const std::string& deviceId, const std::string& neighborId) {
        auto deviceIt = nodes.find(deviceId);
        auto neighborIt = nodes.find(neighborId);
        
        if (deviceIt == nodes.end() || neighborIt == nodes.end()) {
            std::cout << "Cannot add neighbor relationship - device not found" << std::endl;
            return false;
        }
        
        // Add bidirectional relationship
        if (std::find(deviceIt->second.neighbors.begin(), 
                     deviceIt->second.neighbors.end(), neighborId) == deviceIt->second.neighbors.end()) {
            deviceIt->second.neighbors.push_back(neighborId);
        }
        
        if (std::find(neighborIt->second.neighbors.begin(), 
                     neighborIt->second.neighbors.end(), deviceId) == neighborIt->second.neighbors.end()) {
            neighborIt->second.neighbors.push_back(deviceId);
        }
        
        // Update routing table
        updateRoutingTable();
        
        std::cout << "Neighbor relationship established: " << deviceId 
                 << " <-> " << neighborId << std::endl;
        return true;
    }
    
    bool MeshNetwork::removeDevice(const std::string& deviceId) {
        auto it = nodes.find(deviceId);
        if (it == nodes.end()) {
            std::cout << "Device " << deviceId << " not found in mesh network" << std::endl;
            return false;
        }
        
        // Remove this device from all neighbors' lists
        for (const std::string& neighborId : it->second.neighbors) {
            auto neighborIt = nodes.find(neighborId);
            if (neighborIt != nodes.end()) {
                auto& neighbors = neighborIt->second.neighbors;
                neighbors.erase(std::remove(neighbors.begin(), neighbors.end(), deviceId), neighbors.end());
            }
        }
        
        // If this was the gateway, clear gateway ID
        if (deviceId == gatewayId) {
            gatewayId = "";
        }
        
        nodes.erase(it);
        
        // Update routing table
        updateRoutingTable();
        
        std::cout << "Device " << deviceId << " removed from mesh network" << std::endl;
        return true;
    }
    
    std::vector<std::string> MeshNetwork::findOptimalPath(const std::string& sourceDevice) {
        if (gatewayId.empty()) {
            std::cout << "No gateway configured in mesh network" << std::endl;
            return {};
        }
        
        return bfsShortestPath(sourceDevice, gatewayId);
    }
    
    void MeshNetwork::updateRoutingTable() {
        updateHopCounts();
        std::cout << "Mesh network routing table updated" << std::endl;
    }
    
    int MeshNetwork::getHopCount(const std::string& deviceId) const {
        auto it = nodes.find(deviceId);
        if (it != nodes.end()) {
            return it->second.hopCountToGateway;
        }
        return maxHops;  // Unreachable
    }
    
    bool MeshNetwork::canReachGateway(const std::string& deviceId) const {
        return getHopCount(deviceId) < maxHops;
    }
    
    std::vector<std::string> MeshNetwork::getNeighbors(const std::string& deviceId) const {
        auto it = nodes.find(deviceId);
        if (it != nodes.end()) {
            return it->second.neighbors;
        }
        return {};
    }
    
    void MeshNetwork::setGateway(const std::string& deviceId) {
        auto it = nodes.find(deviceId);
        if (it != nodes.end()) {
            // Clear previous gateway
            if (!gatewayId.empty()) {
                auto gatewayIt = nodes.find(gatewayId);
                if (gatewayIt != nodes.end()) {
                    gatewayIt->second.isGateway = false;
                }
            }
            
            // Set new gateway
            it->second.isGateway = true;
            it->second.hopCountToGateway = 0;
            gatewayId = deviceId;
            
            // Update hop counts
            updateHopCounts();
            
            std::cout << "Device " << deviceId << " set as gateway" << std::endl;
        } else {
            std::cout << "Device " << deviceId << " not found in mesh network" << std::endl;
        }
    }
    
    void MeshNetwork::printTopology() const {
        std::cout << "\n=== MESH NETWORK TOPOLOGY ===" << std::endl;
        std::cout << "Gateway: " << (gatewayId.empty() ? "None" : gatewayId) << std::endl;
        std::cout << "Total Devices: " << nodes.size() << std::endl;
        
        for (const auto& pair : nodes) {
            const MeshNode& node = pair.second;
            std::cout << "  " << node.deviceId 
                     << " (Hops: " << node.hopCountToGateway
                     << ", Neighbors: " << node.neighbors.size()
                     << (node.isGateway ? ", GATEWAY" : "") << ")" << std::endl;
            
            if (!node.neighbors.empty()) {
                std::cout << "    Neighbors: ";
                for (size_t i = 0; i < node.neighbors.size(); ++i) {
                    std::cout << node.neighbors[i];
                    if (i < node.neighbors.size() - 1) std::cout << ", ";
                }
                std::cout << std::endl;
            }
        }
        std::cout << "=============================" << std::endl;
    }
    
    void MeshNetwork::printStatistics() const {
        std::cout << "\n=== MESH NETWORK STATISTICS ===" << std::endl;
        
        int totalDevices = nodes.size();
        int reachableDevices = 0;
        int unreachableDevices = 0;
        int gatewayDevices = 0;
        double averageHops = 0.0;
        
        for (const auto& pair : nodes) {
            const MeshNode& node = pair.second;
            if (node.isGateway) {
                gatewayDevices++;
            }
            if (node.hopCountToGateway < maxHops) {
                reachableDevices++;
                if (!node.isGateway) {  // Don't count gateway itself
                    averageHops += node.hopCountToGateway;
                }
            } else {
                unreachableDevices++;
            }
        }
        
        if (reachableDevices > gatewayDevices) {
            averageHops /= (reachableDevices - gatewayDevices);
        }
        
        std::cout << "Total Devices: " << totalDevices << std::endl;
        std::cout << "Reachable Devices: " << reachableDevices << std::endl;
        std::cout << "Unreachable Devices: " << unreachableDevices << std::endl;
        std::cout << "Gateway Devices: " << gatewayDevices << std::endl;
        std::cout << "Average Hops to Gateway: " << (averageHops > 0 ? std::to_string(averageHops) : "N/A") << std::endl;
        std::cout << "===============================" << std::endl;
    }
    
    std::vector<std::string> MeshNetwork::bfsShortestPath(const std::string& start, const std::string& target) {
        if (nodes.find(start) == nodes.end() || nodes.find(target) == nodes.end()) {
            return {};
        }
        
        if (start == target) {
            return {start};
        }
        
        std::map<std::string, bool> visited;
        std::map<std::string, std::string> parent;
        std::queue<std::string> queue;
        
        visited[start] = true;
        queue.push(start);
        
        while (!queue.empty()) {
            std::string current = queue.front();
            queue.pop();
            
            const auto& currentNode = nodes.at(current);
            for (const std::string& neighborId : currentNode.neighbors) {
                if (visited.find(neighborId) == visited.end()) {
                    visited[neighborId] = true;
                    parent[neighborId] = current;
                    queue.push(neighborId);
                    
                    if (neighborId == target) {
                        // Reconstruct path
                        std::vector<std::string> path;
                        std::string node = target;
                        while (node != start) {
                            path.push_back(node);
                            node = parent[node];
                        }
                        path.push_back(start);
                        std::reverse(path.begin(), path.end());
                        return path;
                    }
                }
            }
        }
        
        return {};  // No path found
    }
    
    void MeshNetwork::updateHopCounts() {
        if (gatewayId.empty()) return;
        
        // Initialize all hop counts to max
        for (auto& pair : nodes) {
            if (pair.first != gatewayId) {
                pair.second.hopCountToGateway = maxHops;
            } else {
                pair.second.hopCountToGateway = 0;  // Gateway has 0 hops to itself
            }
        }
        
        // BFS from gateway to calculate hop counts
        std::map<std::string, bool> visited;
        std::queue<std::pair<std::string, int>> queue;  // {deviceId, hopCount}
        
        visited[gatewayId] = true;
        queue.push({gatewayId, 0});
        
        while (!queue.empty()) {
            auto [currentDevice, currentHops] = queue.front();
            queue.pop();
            
            const auto& currentNode = nodes.at(currentDevice);
            int nextHops = currentHops + 1;
            
            for (const std::string& neighborId : currentNode.neighbors) {
                if (visited.find(neighborId) == visited.end() || 
                    nodes[neighborId].hopCountToGateway > nextHops) {
                    
                    nodes[neighborId].hopCountToGateway = nextHops;
                    visited[neighborId] = true;
                    
                    if (nextHops < maxHops) {
                        queue.push({neighborId, nextHops});
                    }
                }
            }
        }
    }
    
} // namespace iot