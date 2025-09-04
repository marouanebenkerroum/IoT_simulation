#ifndef IOT_SIMULATION_MESH_NETWORK_H
#define IOT_SIMULATION_MESH_NETWORK_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <set>

namespace iot {
    
    /**
     * @brief Mesh Network Topology Manager
     */
    class MeshNetwork {
    private:
        struct MeshNode {
            std::string deviceId;
            std::vector<std::string> neighbors;
            int hopCountToGateway;
            bool isGateway;
            double signalStrength;
        };
        
        std::map<std::string, MeshNode> nodes;
        std::string gatewayId;
        int maxHops;
        
    public:
        /**
         * @brief Constructor
         */
        MeshNetwork(int maxHopCount = 10);
        
        /**
         * @brief Add device to mesh network
         */
        bool addDevice(const std::string& deviceId, bool isGatewayNode = false);
        
        /**
         * @brief Add neighbor relationship between devices
         */
        bool addNeighbor(const std::string& deviceId, const std::string& neighborId);
        
        /**
         * @brief Remove device from mesh network
         */
        bool removeDevice(const std::string& deviceId);
        
        /**
         * @brief Find optimal path to gateway
         */
        std::vector<std::string> findOptimalPath(const std::string& sourceDevice);
        
        /**
         * @brief Update routing table using simple flooding algorithm
         */
        void updateRoutingTable();
        
        /**
         * @brief Get hop count to gateway for device
         */
        int getHopCount(const std::string& deviceId) const;
        
        /**
         * @brief Check if device can reach gateway
         */
        bool canReachGateway(const std::string& deviceId) const;
        
        /**
         * @brief Get all neighbors of a device
         */
        std::vector<std::string> getNeighbors(const std::string& deviceId) const;
        
        /**
         * @brief Set gateway device
         */
        void setGateway(const std::string& deviceId);
        
        /**
         * @brief Get gateway device ID
         */
        std::string getGateway() const { return gatewayId; }
        
        /**
         * @brief Print mesh network topology
         */
        void printTopology() const;
        
        /**
         * @brief Get network statistics
         */
        void printStatistics() const;
        
    private:
        /**
         * @brief Calculate shortest path using BFS
         */
        std::vector<std::string> bfsShortestPath(const std::string& start, const std::string& target);
        
        /**
         * @brief Update hop counts for all nodes
         */
        void updateHopCounts();
    };
    
} // namespace iot

#endif // IOT_SIMULATION_MESH_NETWORK_H