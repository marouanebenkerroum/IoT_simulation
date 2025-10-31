#ifndef IOT_SIMULATION_IPSEC_MANAGER_H
#define IOT_SIMULATION_IPSEC_MANAGER_H

#include <string>
#include <map>
#include <memory>
#include <mutex>
#include "../core/Message.h"

namespace iot {
    
    /**
     * @brief IPsec Security Association (simulated)
     */
    struct SecurityAssociation {
        std::string spi;                    // Security Parameter Index
        std::string encryptionKey;          // Encryption key
        std::string authenticationKey;      // Authentication key
        std::string sourceIP;
        std::string destinationIP;
        long long sequenceNumber;
        std::chrono::steady_clock::time_point creationTime;
        std::chrono::steady_clock::time_point expiryTime;
        bool isActive;
    };
    
    /**
     * @brief IPsec Security Policy (simulated)
     */
    struct SecurityPolicy {
        std::string sourceIP;
        std::string destinationIP;
        std::string protocol;               // ESP, AH, or BOTH
        bool requireEncryption;
        bool requireAuthentication;
        int securityLevel;                  // 1-10 scale
    };
    
    /**
     * @brief IPsec Manager for IoT network security
     */
    class IPSecManager {
    public:
        enum class IPsecMode {
            TRANSPORT,      // End-to-end security
            TUNNEL          // Gateway-to-gateway security
        };
        
        enum class EncryptionAlgorithm {
            AES_128_CBC,
            AES_256_CBC,
            AES_128_GCM,
            AES_256_GCM,
            NULL_ENCRYPTION  // No encryption (for testing)
        };
        
        enum class AuthenticationAlgorithm {
            HMAC_SHA256,
            HMAC_SHA384,
            HMAC_SHA512,
            NULL_AUTH       // No authentication (for testing)
        };
        
    private:
        std::map<std::string, SecurityAssociation> securityAssociations;
        std::map<std::string, SecurityPolicy> securityPolicies;
        std::mutex ipsecMutex;
        IPsecMode defaultMode;
        EncryptionAlgorithm defaultEncryption;
        AuthenticationAlgorithm defaultAuthentication;
        bool isEnabled;
        
    public:
        /**
         * @brief Constructor
         */
        IPSecManager(IPsecMode mode = IPsecMode::TRANSPORT);
        
        /**
         * @brief Destructor
         */
        ~IPSecManager() = default;
        
        /**
         * @brief Enable/disable IPsec
         */
        void setEnabled(bool enabled);
        bool isEnabledIPSec() const { return isEnabled; }
        
        /**
         * @brief Create a security association
         */
        bool createSecurityAssociation(const std::string& sourceIP, 
                                     const std::string& destinationIP,
                                     const std::string& spi = "");
        
        /**
         * @brief Remove a security association
         */
        bool removeSecurityAssociation(const std::string& spi);
        
        /**
         * @brief Add security policy
         */
        bool addSecurityPolicy(const std::string& sourceIP, 
                             const std::string& destinationIP,
                             const SecurityPolicy& policy);
        
        /**
         * @brief Check if communication should be secured
         */
        bool shouldSecureCommunication(const std::string& sourceIP, 
                                     const std::string& destinationIP) const;
        
        /**
         * @brief Encrypt and authenticate message (ESP simulation)
         */
        std::string encryptAndAuthenticate(const std::string& payload,
                                         const std::string& sourceIP,
                                         const std::string& destinationIP);
        
        /**
         * @brief Decrypt and verify message (ESP simulation)
         */
        std::string decryptAndVerify(const std::string& encryptedPayload,
                                   const std::string& sourceIP,
                                   const std::string& destinationIP);
        
        /**
         * @brief Authenticate only (AH simulation)
         */
        std::string authenticateOnly(const std::string& payload,
                                   const std::string& sourceIP,
                                   const std::string& destinationIP);
        
        /**
         * @brief Verify authentication only (AH simulation)
         */
        bool verifyAuthentication(const std::string& payload,
                                const std::string& signature,
                                const std::string& sourceIP,
                                const std::string& destinationIP);
        
        /**
         * @brief Generate security parameter index
         */
        std::string generateSPI() const;
        
        /**
         * @brief Generate cryptographic keys
         */
        std::string generateEncryptionKey(EncryptionAlgorithm algo) const;
        std::string generateAuthenticationKey(AuthenticationAlgorithm algo) const;
        
        /**
         * @brief Get security association by SPI
         */
        const SecurityAssociation* getSecurityAssociation(const std::string& spi) const;
        
        /**
         * @brief Print IPsec statistics
         */
        void printIPSecStatistics() const;
        
        /**
         * @brief Cleanup expired security associations
         */
        void cleanupExpiredSAs();
        
    private:
        /**
         * @brief Find existing SA for communication pair
         */
        const SecurityAssociation* findSAForCommunication(const std::string& sourceIP,
                                                        const std::string& destinationIP) const;
        
        // Diffie-Hellman Key Exchange
        struct DHParams {
            uint64_t prime;      // Large prime p
            uint64_t generator; // Generator g
            uint64_t privateKey; // Private key a
            uint64_t publicKey; // Public key A = g^a mod p
        };
        
        /**
         * @brief Perform Diffie-Hellman key exchange
         * @param sourceIP Source IP address
         * @param destIP Destination IP address
         * @return Pair of (encryptionKey, authenticationKey) derived from shared secret
         */
        std::pair<std::string, std::string> performDHKeyExchange(
            const std::string& sourceIP,
            const std::string& destIP) const;
        
        /**
         * @brief Compute modular exponentiation (base^exp mod modulus)
         */
        uint64_t modPow(uint64_t base, uint64_t exp, uint64_t modulus) const;
        
        /**
         * @brief Derive keys from shared secret using HKDF-like approach
         */
        std::pair<std::string, std::string> deriveKeysFromSharedSecret(
            uint64_t sharedSecret, EncryptionAlgorithm encAlgo, AuthenticationAlgorithm authAlgo) const;
        
        /**
         * @brief AES-like encryption (simulated with stronger algorithm)
         */
        std::string aesEncrypt(const std::string& data, const std::string& key) const;
        
        /**
         * @brief AES-like decryption (simulated with stronger algorithm)
         */
        std::string aesDecrypt(const std::string& data, const std::string& key) const;
        
        /**
         * @brief SHA-based HMAC computation
         */
        std::string computeHMAC(const std::string& data, const std::string& key, 
                               AuthenticationAlgorithm algo) const;
        
        /**
         * @brief Verify HMAC signature
         */
        bool verifyHMAC(const std::string& data, 
                       const std::string& signature, 
                       const std::string& key,
                       AuthenticationAlgorithm algo) const;
        
        /**
         * @brief SHA-256 hash (simulated)
         */
        std::string sha256(const std::string& data) const;
        
        /**
         * @brief SHA-512 hash (simulated)
         */
        std::string sha512(const std::string& data) const;
    };
    
} // namespace iot

#endif // IOT_SIMULATION_IPSEC_MANAGER_H