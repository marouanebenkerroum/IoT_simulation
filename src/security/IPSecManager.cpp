#include "../../include/security/IPSecManager.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <random>
#include <algorithm>
#include <chrono>

namespace iot {
    
    IPSecManager::IPSecManager(IPsecMode mode)
        : defaultMode(mode)
        , defaultEncryption(EncryptionAlgorithm::AES_128_CBC)
        , defaultAuthentication(AuthenticationAlgorithm::HMAC_SHA256)
        , isEnabled(true) {
        std::cout << "IPsec Manager initialized in " 
                  << (mode == IPsecMode::TRANSPORT ? "Transport" : "Tunnel") 
                  << " mode" << std::endl;
    }
    
    void IPSecManager::setEnabled(bool enabled) {
        std::lock_guard<std::mutex> lock(ipsecMutex);
        isEnabled = enabled;
        std::cout << "IPsec " << (enabled ? "enabled" : "disabled") << std::endl;
    }
    
    bool IPSecManager::createSecurityAssociation(const std::string& sourceIP, 
                                               const std::string& destinationIP,
                                               const std::string& spi) {
        std::lock_guard<std::mutex> lock(ipsecMutex);
        
        std::string actualSPI = spi.empty() ? generateSPI() : spi;
        
        SecurityAssociation sa;
        sa.spi = actualSPI;
        sa.sourceIP = sourceIP;
        sa.destinationIP = destinationIP;
        sa.encryptionKey = generateEncryptionKey(defaultEncryption);
        sa.authenticationKey = generateAuthenticationKey(defaultAuthentication);
        sa.sequenceNumber = 1;
        sa.creationTime = std::chrono::steady_clock::now();
        sa.expiryTime = sa.creationTime + std::chrono::hours(24);  // 24-hour lifetime
        sa.isActive = true;
        
        securityAssociations[actualSPI] = sa;
        
        std::cout << "Created IPsec SA: " << actualSPI 
                  << " (" << sourceIP << " <-> " << destinationIP << ")" << std::endl;
        
        return true;
    }
    
    bool IPSecManager::removeSecurityAssociation(const std::string& spi) {
        std::lock_guard<std::mutex> lock(ipsecMutex);
        
        auto it = securityAssociations.find(spi);
        if (it != securityAssociations.end()) {
            it->second.isActive = false;
            std::cout << "Removed IPsec SA: " << spi << std::endl;
            return true;
        }
        
        std::cout << "IPsec SA not found: " << spi << std::endl;
        return false;
    }
    
    bool IPSecManager::addSecurityPolicy(const std::string& sourceIP, 
                                       const std::string& destinationIP,
                                       const SecurityPolicy& policy) {
        std::lock_guard<std::mutex> lock(ipsecMutex);
        
        std::string policyKey = sourceIP + "->" + destinationIP;
        securityPolicies[policyKey] = policy;
        
        std::cout << "Added IPsec policy for " << policyKey << std::endl;
        return true;
    }
    
    bool IPSecManager::shouldSecureCommunication(const std::string& sourceIP, 
                                               const std::string& destinationIP) const {
        if (!isEnabled) return false;
        
        std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(ipsecMutex));
        
        std::string policyKey = sourceIP + "->" + destinationIP;
        auto it = securityPolicies.find(policyKey);
        if (it != securityPolicies.end()) {
            return it->second.requireEncryption || it->second.requireAuthentication;
        }
        
        // Default policy: secure communication between different networks
        return sourceIP != destinationIP;
    }
    
    std::string IPSecManager::encryptAndAuthenticate(const std::string& payload,
                                                   const std::string& sourceIP,
                                                   const std::string& destinationIP) {
        if (!isEnabled || !shouldSecureCommunication(sourceIP, destinationIP)) {
            return payload;  // No security applied
        }
        
        std::lock_guard<std::mutex> lock(ipsecMutex);
        
        // Find or create security association
        const SecurityAssociation* sa = findSAForCommunication(sourceIP, destinationIP);
        if (!sa) {
            // Create new SA if none exists
            createSecurityAssociation(sourceIP, destinationIP);
            sa = findSAForCommunication(sourceIP, destinationIP);
            if (!sa) {
                std::cerr << "Failed to create IPsec SA for " << sourceIP 
                         << " -> " << destinationIP << std::endl;
                return payload;
            }
        }
        
        // Encrypt payload
        std::string encryptedPayload = simpleEncrypt(payload, sa->encryptionKey);
        
        // Add authentication (HMAC)
        std::string hmac = simpleHMAC(encryptedPayload, sa->authenticationKey);
        
        // Create ESP-like packet format
        std::ostringstream oss;
        oss << "ESP[" << sa->spi << "|" << sa->sequenceNumber << "|" 
            << encryptedPayload << "|HMAC:" << hmac << "]";
        
        // Increment sequence number
        const_cast<SecurityAssociation*>(sa)->sequenceNumber++;
        
        std::cout << "IPsec ESP applied: " << sourceIP << " -> " << destinationIP 
                  << " (SPI: " << sa->spi << ")" << std::endl;
        
        return oss.str();
    }
    
    std::string IPSecManager::decryptAndVerify(const std::string& encryptedPayload,
                                             const std::string& sourceIP,
                                             const std::string& destinationIP) {
        if (!isEnabled) return encryptedPayload;
        
        std::lock_guard<std::mutex> lock(ipsecMutex);
        
        // Parse ESP packet format (simplified)
        if (encryptedPayload.substr(0, 4) != "ESP[") {
            return encryptedPayload;  // Not an ESP packet
        }
        
        // Extract SPI from packet
        size_t spiStart = encryptedPayload.find('[') + 1;
        size_t spiEnd = encryptedPayload.find('|', spiStart);
        std::string spi = encryptedPayload.substr(spiStart, spiEnd - spiStart);
        
        // Find security association
        auto it = securityAssociations.find(spi);
        if (it == securityAssociations.end() || !it->second.isActive) {
            std::cerr << "Invalid or expired IPsec SA: " << spi << std::endl;
            return "";
        }
        
        const SecurityAssociation& sa = it->second;
        
        // Extract encrypted data and HMAC
        size_t dataStart = spiEnd + 1;
        size_t seqEnd = encryptedPayload.find('|', dataStart);
        // Skip sequence number
        size_t hmacStart = encryptedPayload.rfind("|HMAC:") + 6;
        size_t hmacEnd = encryptedPayload.length() - 1;  // Remove closing ']'
        
        std::string encryptedData = encryptedPayload.substr(seqEnd + 1, hmacStart - seqEnd - 7);
        std::string receivedHMAC = encryptedPayload.substr(hmacStart, hmacEnd - hmacStart);
        
        // Verify HMAC
        std::string calculatedHMAC = simpleHMAC(encryptedData, sa.authenticationKey);
        if (calculatedHMAC != receivedHMAC) {
            std::cerr << "IPsec authentication failed for SPI: " << spi << std::endl;
            return "";
        }
        
        // Decrypt payload
        std::string decryptedPayload = simpleDecrypt(encryptedData, sa.encryptionKey);
        
        std::cout << "IPsec ESP verified and decrypted: " << sourceIP << " -> " << destinationIP 
                  << " (SPI: " << spi << ")" << std::endl;
        
        return decryptedPayload;
    }
    
    std::string IPSecManager::authenticateOnly(const std::string& payload,
                                             const std::string& sourceIP,
                                             const std::string& destinationIP) {
        if (!isEnabled || !shouldSecureCommunication(sourceIP, destinationIP)) {
            return payload;
        }
        
        std::lock_guard<std::mutex> lock(ipsecMutex);
        
        const SecurityAssociation* sa = findSAForCommunication(sourceIP, destinationIP);
        if (!sa) {
            createSecurityAssociation(sourceIP, destinationIP);
            sa = findSAForCommunication(sourceIP, destinationIP);
            if (!sa) return payload;
        }
        
        std::string hmac = simpleHMAC(payload, sa->authenticationKey);
        
        // Create AH-like packet format
        std::ostringstream oss;
        oss << "AH[" << sa->spi << "|" << sa->sequenceNumber << "|" 
            << payload << "|HMAC:" << hmac << "]";
        
        const_cast<SecurityAssociation*>(sa)->sequenceNumber++;
        
        std::cout << "IPsec AH applied: " << sourceIP << " -> " << destinationIP 
                  << " (SPI: " << sa->spi << ")" << std::endl;
        
        return oss.str();
    }
    
    bool IPSecManager::verifyAuthentication(const std::string& payload,
                                          const std::string& signature,
                                          const std::string& sourceIP,
                                          const std::string& destinationIP) {
        if (!isEnabled) return true;
        
        std::lock_guard<std::mutex> lock(ipsecMutex);
        
        const SecurityAssociation* sa = findSAForCommunication(sourceIP, destinationIP);
        if (!sa) return false;
        
        return verifyHMAC(payload, signature, sa->authenticationKey);
    }
    
    std::string IPSecManager::generateSPI() const {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::uniform_int_distribution<> dis(10000000, 99999999);
        
        return "SPI" + std::to_string(dis(gen));
    }
    
    std::string IPSecManager::generateEncryptionKey(EncryptionAlgorithm algo) const {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::uniform_int_distribution<> dis(0, 255);
        
        size_t keyLength = 16;  // 128 bits default
        switch (algo) {
            case EncryptionAlgorithm::AES_128_CBC:
            case EncryptionAlgorithm::AES_128_GCM:
                keyLength = 16;
                break;
            case EncryptionAlgorithm::AES_256_CBC:
            case EncryptionAlgorithm::AES_256_GCM:
                keyLength = 32;
                break;
            default:
                keyLength = 16;
        }
        
        std::string key;
        for (size_t i = 0; i < keyLength; ++i) {
            key += static_cast<char>(dis(gen));
        }
        
        return key;
    }
    
    std::string IPSecManager::generateAuthenticationKey(AuthenticationAlgorithm algo) const {
        return generateEncryptionKey(EncryptionAlgorithm::AES_128_CBC);  // Simplified
    }
    
    const SecurityAssociation* IPSecManager::getSecurityAssociation(const std::string& spi) const {
        std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(ipsecMutex));
        
        auto it = securityAssociations.find(spi);
        if (it != securityAssociations.end() && it->second.isActive) {
            return &it->second;
        }
        return nullptr;
    }
    
    void IPSecManager::printIPSecStatistics() const {
        std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(ipsecMutex));
        
        std::cout << "\n=== IPsec Statistics ===" << std::endl;
        std::cout << "Status: " << (isEnabled ? "ENABLED" : "DISABLED") << std::endl;
        std::cout << "Active SAs: " << securityAssociations.size() << std::endl;
        std::cout << "Security Policies: " << securityPolicies.size() << std::endl;
        std::cout << "Default Mode: " << (defaultMode == IPsecMode::TRANSPORT ? "Transport" : "Tunnel") << std::endl;
        
        for (const auto& pair : securityAssociations) {
            const SecurityAssociation& sa = pair.second;
            if (sa.isActive) {
                std::cout << "  SA " << sa.spi << ": " << sa.sourceIP 
                         << " <-> " << sa.destinationIP << std::endl;
            }
        }
        std::cout << "=====================" << std::endl;
    }
    
    void IPSecManager::cleanupExpiredSAs() {
        std::lock_guard<std::mutex> lock(ipsecMutex);
        
        auto now = std::chrono::steady_clock::now();
        size_t removed = 0;
        
        for (auto it = securityAssociations.begin(); it != securityAssociations.end();) {
            if (now > it->second.expiryTime) {
                it = securityAssociations.erase(it);
                removed++;
            } else {
                ++it;
            }
        }
        
        if (removed > 0) {
            std::cout << "Cleaned up " << removed << " expired IPsec SAs" << std::endl;
        }
    }
    
    const SecurityAssociation* IPSecManager::findSAForCommunication(const std::string& sourceIP,
                                                                  const std::string& destinationIP) const {
        for (const auto& pair : securityAssociations) {
            const SecurityAssociation& sa = pair.second;
            if (sa.isActive && 
                ((sa.sourceIP == sourceIP && sa.destinationIP == destinationIP) ||
                 (sa.sourceIP == destinationIP && sa.destinationIP == sourceIP))) {
                return &sa;
            }
        }
        return nullptr;
    }
    
    std::string IPSecManager::simpleEncrypt(const std::string& data, const std::string& key) const {
        // Simple XOR encryption for simulation purposes
        std::string encrypted = data;
        for (size_t i = 0; i < data.length(); ++i) {
            encrypted[i] = data[i] ^ key[i % key.length()];
        }
        return encrypted;
    }
    
    std::string IPSecManager::simpleDecrypt(const std::string& data, const std::string& key) const {
        // XOR decryption is symmetric
        return simpleEncrypt(data, key);
    }
    
    std::string IPSecManager::simpleHMAC(const std::string& data, const std::string& key) const {
        // Simple hash-based message authentication for simulation
        std::hash<std::string> hasher;
        size_t hash1 = hasher(data);
        size_t hash2 = hasher(key);
        size_t combined = hash1 ^ hash2;
        
        std::ostringstream oss;
        oss << std::hex << std::setfill('0') << std::setw(8) << combined;
        return oss.str();
    }
    
    bool IPSecManager::verifyHMAC(const std::string& data, 
                                const std::string& signature, 
                                const std::string& key) const {
        std::string calculated = simpleHMAC(data, key);
        return calculated == signature;
    }
    
} // namespace iot