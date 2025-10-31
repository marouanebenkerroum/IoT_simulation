#include "../../include/security/SecurityManager.h"
#include <iostream>
#include <random>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <mutex>

namespace iot {
    
    static std::mutex securityMutex;

    SecurityManager::SecurityManager(SecurityLevel defaultLevel)
        : defaultSecurityLevel(defaultLevel) {
        std::cout << "Security Manager initialized with default level: " 
                  << static_cast<int>(defaultLevel) << std::endl;
    }
    
    std::pair<bool, std::string> SecurityManager::registerDevice(const std::string& deviceId, SecurityLevel level) {
        if (deviceId.empty()) {
            std::cerr << "Error: Cannot register device with empty ID" << std::endl;
            return {false, ""};
        }

        std::lock_guard<std::mutex> lock(securityMutex);
        
        // Check if device is already registered
        if (deviceSecurity.find(deviceId) != deviceSecurity.end()) {
            std::cout << "Device " << deviceId << " is already registered" << std::endl;
            return {false,""};
        }

        DeviceSecurityInfo info;
        info.deviceId = deviceId;
        info.securityLevel = level;
        info.isAuthenticated = false;
        
        // Generate secure auth token
        static std::random_device rd;
        static std::mt19937_64 gen(rd());  // Use 64-bit Mersenne Twister
        static std::uniform_int_distribution<uint64_t> dis;
        
        // Create a more complex token with timestamp
        auto now = std::chrono::system_clock::now();
        auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()
        ).count();
        
        std::stringstream tokenStream;
        tokenStream << std::hex << timestamp << "_" << dis(gen);
        info.authToken = "TOKEN_" + tokenStream.str();
        
        // Generate encryption key using more entropy
        std::string keyMaterial;
        for (int i = 0; i < 32; ++i) {
            keyMaterial += static_cast<char>(dis(gen) & 0xFF);
        }
        info.encryptionKey = keyMaterial;
        
        deviceSecurity[deviceId] = info;
        
        std::cout << "Device " << deviceId << " registered with security level " 
                  << static_cast<int>(level) << std::endl;
        return {true, info.authToken};
    }
    
    // Add this method implementation
std::string SecurityManager::getDeviceToken(const std::string& deviceId) const {
    std::lock_guard<std::mutex> lock(securityMutex);
    
    auto it = deviceSecurity.find(deviceId);
    if (it != deviceSecurity.end()) {
        return it->second.authToken;
    }
    
    return "";  // Return empty string if device not found
}
    bool SecurityManager::authenticateDevice(const std::string& deviceId, const std::string& token) {
        if (deviceId.empty() || token.empty()) {
            std::cerr << "Error: Device ID and token cannot be empty" << std::endl;
            return false;
        }

        std::lock_guard<std::mutex> lock(securityMutex);
        
        auto it = deviceSecurity.find(deviceId);
        if (it == deviceSecurity.end()) {
            std::cerr << "Error: Device " << deviceId << " not registered" << std::endl;
            return false;
        }

        // Constant-time string comparison to prevent timing attacks
        if (it->second.authToken.length() != token.length()) {
            std::cout << "Device " << deviceId << " authentication failed" << std::endl;
            return false;
        }

        bool authenticated = true;
        for (size_t i = 0; i < token.length(); i++) {
            if (it->second.authToken[i] != token[i]) {
                authenticated = false;
            }
        }

        if (authenticated) {
            it->second.isAuthenticated = true;
            // Update last authentication time
            it->second.lastAuthTime = std::chrono::system_clock::now();
            std::cout << "Device " << deviceId << " authenticated successfully" << std::endl;
            return true;
        }

        std::cout << "Device " << deviceId << " authentication failed" << std::endl;
        return false;
    }
    
    bool SecurityManager::isAuthorizedToSend(const std::string& deviceId) const {
        auto it = deviceSecurity.find(deviceId);
        if (it != deviceSecurity.end()) {
            return it->second.isAuthenticated;
        }
        return false;  // Not registered devices are not authorized
    }
    
    std::string SecurityManager::encryptMessage(const std::string& message, const std::string& deviceId) {
        auto it = deviceSecurity.find(deviceId);
        if (it != deviceSecurity.end() && it->second.isAuthenticated) {
            // Simple "encryption" for demonstration (XOR with key)
            std::string key = it->second.encryptionKey;
            std::string encrypted = message;
            
            // Simple XOR encryption (for demonstration only)
            for (size_t i = 0; i < message.length(); ++i) {
                encrypted[i] = message[i] ^ key[i % key.length()];
            }
            
            return encrypted;
        }
        return message;  // Return unencrypted if not authorized
    }
    
    std::string SecurityManager::decryptMessage(const std::string& encryptedMessage, const std::string& deviceId) {
        return encryptMessage(encryptedMessage, deviceId);  // XOR is symmetric
    }
    
    SecurityManager::SecurityLevel SecurityManager::getDeviceSecurityLevel(const std::string& deviceId) const {
        auto it = deviceSecurity.find(deviceId);
        if (it != deviceSecurity.end()) {
            return it->second.securityLevel;
        }
        return defaultSecurityLevel;
    }
    
    void SecurityManager::printSecurityReport() const {
        std::cout << "\n=== SECURITY REPORT ===" << std::endl;
        std::cout << "Registered Devices: " << deviceSecurity.size() << std::endl;
        
        for (const auto& pair : deviceSecurity) {
            const DeviceSecurityInfo& info = pair.second;
            std::cout << "  Device: " << info.deviceId << std::endl;
            std::cout << "    Security Level: " << static_cast<int>(info.securityLevel) << std::endl;
            std::cout << "    Authenticated: " << (info.isAuthenticated ? "YES" : "NO") << std::endl;
        }
        std::cout << "=====================" << std::endl;
    }
    
} // namespace iot