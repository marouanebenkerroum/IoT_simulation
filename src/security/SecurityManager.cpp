#include "../../include/security/SecurityManager.h"
#include <iostream>
#include <random>
#include <algorithm>
#include <iomanip>
#include <sstream>

namespace iot {
    
    SecurityManager::SecurityManager(SecurityLevel defaultLevel)
        : defaultSecurityLevel(defaultLevel) {
        std::cout << "Security Manager initialized with default level: " 
                  << static_cast<int>(defaultLevel) << std::endl;
    }
    
    bool SecurityManager::registerDevice(const std::string& deviceId, SecurityLevel level) {
        DeviceSecurityInfo info;
        info.deviceId = deviceId;
        info.securityLevel = level;
        info.isAuthenticated = false;
        
        // Generate simple auth token (in reality, this would be more secure)
        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::uniform_int_distribution<> dis(100000, 999999);
        info.authToken = "TOKEN_" + std::to_string(dis(gen));
        
        // Generate simple encryption key
        info.encryptionKey = "KEY_" + deviceId.substr(0, 8);
        
        deviceSecurity[deviceId] = info;
        
        std::cout << "Device " << deviceId << " registered with security level " 
                  << static_cast<int>(level) << std::endl;
        return true;
    }
    
    bool SecurityManager::authenticateDevice(const std::string& deviceId, const std::string& token) {
        auto it = deviceSecurity.find(deviceId);
        if (it != deviceSecurity.end()) {
            if (it->second.authToken == token) {
                it->second.isAuthenticated = true;
                std::cout << "Device " << deviceId << " authenticated successfully" << std::endl;
                return true;
            } else {
                std::cout << "Device " << deviceId << " authentication failed" << std::endl;
                return false;
            }
        }
        std::cout << "Device " << deviceId << " not found for authentication" << std::endl;
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