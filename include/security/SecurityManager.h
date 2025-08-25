#ifndef IOT_SIMULATION_SECURITY_MANAGER_H
#define IOT_SIMULATION_SECURITY_MANAGER_H

#include <string>
#include <map>
#include <memory>

namespace iot {
    
    /**
     * @brief Basic security manager for IoT device authentication and encryption simulation
     */
    class SecurityManager {
    public:
        enum class SecurityLevel {
            NONE,
            BASIC,
            ENHANCED,
            ENTERPRISE
        };
        
        struct DeviceSecurityInfo {
            std::string deviceId;
            std::string authToken;
            SecurityLevel securityLevel;
            bool isAuthenticated;
            std::string encryptionKey;
        };
        
    private:
        std::map<std::string, DeviceSecurityInfo> deviceSecurity;
        SecurityLevel defaultSecurityLevel;
        
    public:
        /**
         * @brief Constructor
         */
        SecurityManager(SecurityLevel defaultLevel = SecurityLevel::BASIC);
        
        /**
         * @brief Register a device for security
         */
        bool registerDevice(const std::string& deviceId, SecurityLevel level = SecurityLevel::BASIC);
        
        /**
         * @brief Authenticate a device
         */
        bool authenticateDevice(const std::string& deviceId, const std::string& token);
        
        /**
         * @brief Check if device is authorized to send message
         */
        bool isAuthorizedToSend(const std::string& deviceId) const;
        
        /**
         * @brief Simulate message encryption
         */
        std::string encryptMessage(const std::string& message, const std::string& deviceId);
        
        /**
         * @brief Simulate message decryption
         */
        std::string decryptMessage(const std::string& encryptedMessage, const std::string& deviceId);
        
        /**
         * @brief Get device security level
         */
        SecurityLevel getDeviceSecurityLevel(const std::string& deviceId) const;
        
        /**
         * @brief Print security report
         */
        void printSecurityReport() const;
    };
    
} // namespace iot

#endif // IOT_SIMULATION_SECURITY_MANAGER_H