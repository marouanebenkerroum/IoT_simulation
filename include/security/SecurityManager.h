#ifndef IOT_SIMULATION_SECURITY_MANAGER_H
#define IOT_SIMULATION_SECURITY_MANAGER_H

#include <string>
#include <map>
#include <memory>
#include <mutex>
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
            std::chrono::system_clock::time_point lastAuthTime;
        };
        
    private:
        std::map<std::string, DeviceSecurityInfo> deviceSecurity;
        SecurityLevel defaultSecurityLevel;
        mutable std::mutex securityMutex;  // Protect shared state
        
    public:
        /**
         * @brief Constructor
         */
        SecurityManager(SecurityLevel defaultLevel = SecurityLevel::BASIC);
        std::string getDeviceToken(const std::string& deviceId) const;
        /**
         * @brief Register a device for security
         * @return pair of success boolean and authentication token
         */
        std::pair<bool, std::string> registerDevice(const std::string& deviceId, SecurityLevel level = SecurityLevel::BASIC);
        
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