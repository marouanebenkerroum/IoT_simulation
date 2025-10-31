#include "../../include/security/IPSecManager.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <random>
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cstddef>

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
        
        // Perform Diffie-Hellman key exchange to derive keys
        auto [encryptionKey, authenticationKey] = performDHKeyExchange(sourceIP, destinationIP);
        
        SecurityAssociation sa;
        sa.spi = actualSPI;
        sa.sourceIP = sourceIP;
        sa.destinationIP = destinationIP;
        sa.encryptionKey = encryptionKey;
        sa.authenticationKey = authenticationKey;
        sa.sequenceNumber = 1;
        sa.creationTime = std::chrono::steady_clock::now();
        sa.expiryTime = sa.creationTime + std::chrono::hours(24);  // 24-hour lifetime
        sa.isActive = true;
        
        securityAssociations[actualSPI] = sa;
        
        std::cout << "Created IPsec SA: " << actualSPI 
                  << " (" << sourceIP << " <-> " << destinationIP << ") with DH key exchange" << std::endl;
        
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
            // Create new SA if none exists (inline creation to avoid deadlock)
            std::string actualSPI = generateSPI();
            
            // Perform Diffie-Hellman key exchange to derive keys
            auto [encryptionKey, authenticationKey] = performDHKeyExchange(sourceIP, destinationIP);
            
            SecurityAssociation newSA;
            newSA.spi = actualSPI;
            newSA.sourceIP = sourceIP;
            newSA.destinationIP = destinationIP;
            newSA.encryptionKey = encryptionKey;
            newSA.authenticationKey = authenticationKey;
            newSA.sequenceNumber = 1;
            newSA.creationTime = std::chrono::steady_clock::now();
            newSA.expiryTime = newSA.creationTime + std::chrono::hours(24);
            newSA.isActive = true;
            
            securityAssociations[actualSPI] = newSA;
            
            std::cout << "Created IPsec SA: " << actualSPI 
                      << " (" << sourceIP << " <-> " << destinationIP << ") with DH key exchange" << std::endl;
            
            sa = &securityAssociations[actualSPI];
        }
        
        // Encrypt payload using AES-like encryption with DH-derived key
        std::string encryptedPayload = aesEncrypt(payload, sa->encryptionKey);
        
        // Add authentication (HMAC) using DH-derived authentication key
        std::string hmac = computeHMAC(encryptedPayload, sa->authenticationKey, defaultAuthentication);
        
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
        
        // Verify HMAC using DH-derived authentication key
        std::string calculatedHMAC = computeHMAC(encryptedData, sa.authenticationKey, defaultAuthentication);
        if (calculatedHMAC != receivedHMAC) {
            std::cerr << "IPsec authentication failed for SPI: " << spi << std::endl;
            return "";
        }
        
        // Decrypt payload using AES-like decryption with DH-derived key
        std::string decryptedPayload = aesDecrypt(encryptedData, sa.encryptionKey);
        
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
        
        std::string hmac = computeHMAC(payload, sa->authenticationKey, defaultAuthentication);
        
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
        
        return verifyHMAC(payload, signature, sa->authenticationKey, defaultAuthentication);
    }
    
    std::string IPSecManager::generateSPI() const {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::uniform_int_distribution<> dis(10000000, 99999999);
        
        return "SPI" + std::to_string(dis(gen));
    }
    
    std::string IPSecManager::generateEncryptionKey(EncryptionAlgorithm algo) const {
        // This method is kept for backward compatibility but DH is preferred
        // It's now mainly used for fallback scenarios
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
        // This method is kept for backward compatibility but DH is preferred
        return generateEncryptionKey(EncryptionAlgorithm::AES_128_CBC);
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
    
    // ============= Diffie-Hellman Key Exchange Implementation =============
    
    std::pair<std::string, std::string> IPSecManager::performDHKeyExchange(
        const std::string& sourceIP,
        const std::string& destIP) const {
        
        // Use well-known DH parameters (smaller values for simulation)
        // In production, these would be much larger (2048+ bits)
        const uint64_t prime = 0xFFFFFFFFFFFFFFFFULL - 58;  // Large prime near 2^64
        const uint64_t generator = 2;  // Generator g
        
        // Generate private keys based on IP addresses (deterministic for simulation)
        // In real implementation, these would be truly random
        std::hash<std::string> hasher;
        uint64_t sourceHash = hasher(sourceIP);
        uint64_t destHash = hasher(destIP);
        
        // Private keys (simulated - in production would be random)
        uint64_t sourcePrivate = (sourceHash % (prime - 2)) + 1;
        uint64_t destPrivate = (destHash % (prime - 2)) + 1;
        
        // Compute public keys
        uint64_t sourcePublic = modPow(generator, sourcePrivate, prime);
        uint64_t destPublic = modPow(generator, destPrivate, prime);
        
        // Compute shared secrets (both parties compute the same value)
        uint64_t sourceSharedSecret = modPow(destPublic, sourcePrivate, prime);
        uint64_t destSharedSecret = modPow(sourcePublic, destPrivate, prime);
        
        // They should be equal (they are in this simulation)
        uint64_t sharedSecret = sourceSharedSecret;  // Both compute the same value
        
        std::cout << "DH Key Exchange: " << sourceIP << " <-> " << destIP 
                  << " (shared secret computed)" << std::endl;
        
        // Derive encryption and authentication keys from shared secret
        return deriveKeysFromSharedSecret(sharedSecret, defaultEncryption, defaultAuthentication);
    }
    
    uint64_t IPSecManager::modPow(uint64_t base, uint64_t exp, uint64_t modulus) const {
        // Modular exponentiation: base^exp mod modulus
        uint64_t result = 1;
        base = base % modulus;
        
        while (exp > 0) {
            if (exp % 2 == 1) {
                result = (result * base) % modulus;
            }
            exp = exp >> 1;
            base = (base * base) % modulus;
        }
        return result;
    }
    
    std::pair<std::string, std::string> IPSecManager::deriveKeysFromSharedSecret(
        uint64_t sharedSecret, EncryptionAlgorithm encAlgo, AuthenticationAlgorithm authAlgo) const {
        
        // Convert shared secret to string
        std::ostringstream ssStream;
        ssStream << std::hex << sharedSecret;
        std::string secretStr = ssStream.str();
        
        // Derive encryption key using SHA-256
        std::string encKeyMaterial = "ENC_KEY_" + secretStr;
        std::string encKey = sha256(encKeyMaterial);
        
        // Derive authentication key using SHA-256
        std::string authKeyMaterial = "AUTH_KEY_" + secretStr;
        std::string authKey = sha256(authKeyMaterial);
        
        // Truncate/pad keys to required length
        size_t encKeyLength = (encAlgo == EncryptionAlgorithm::AES_256_CBC || 
                             encAlgo == EncryptionAlgorithm::AES_256_GCM) ? 32 : 16;
        size_t authKeyLength = 32;  // Standard HMAC key length
        
        encKey = encKey.substr(0, std::min(encKeyLength, encKey.length()));
        if (encKey.length() < encKeyLength) {
            encKey.append(encKeyLength - encKey.length(), '\0');
        }
        
        authKey = authKey.substr(0, std::min(authKeyLength, authKey.length()));
        if (authKey.length() < authKeyLength) {
            authKey.append(authKeyLength - authKey.length(), '\0');
        }
        
        return {encKey, authKey};
    }
    
    // ============= AES-like Encryption Implementation =============
    
    std::string IPSecManager::aesEncrypt(const std::string& data, const std::string& key) const {
        if (key.empty() || data.empty()) return data;
        
        // Simulated AES-CBC encryption using a more sophisticated algorithm
        // Uses multiple rounds of transformations with the key
        std::string encrypted = data;
        
        // Pad data to block size (16 bytes for AES)
        size_t blockSize = 16;
        size_t padding = blockSize - (data.length() % blockSize);
        encrypted.append(padding, static_cast<char>(padding));
        
        // Apply encryption rounds (simulating AES rounds)
        for (size_t round = 0; round < 10; ++round) {
            for (size_t i = 0; i < encrypted.length(); ++i) {
                // Combine with key, shift, and XOR operations
                uint8_t byte = static_cast<uint8_t>(encrypted[i]);
                uint8_t keyByte = static_cast<uint8_t>(key[i % key.length()]);
                
                // SubBytes-like transformation
                byte = ((byte ^ keyByte) + (byte << 1)) ^ (keyByte << 3);
                
                // MixColumns-like transformation
                if (i > 0) {
                    byte ^= static_cast<uint8_t>(encrypted[i-1]);
                }
                
                encrypted[i] = static_cast<char>(byte);
            }
        }
        
        return encrypted;
    }
    
    std::string IPSecManager::aesDecrypt(const std::string& data, const std::string& key) const {
        if (key.empty() || data.empty()) return data;
        
        std::string decrypted = data;
        
        // Apply decryption rounds (reverse of encryption)
        for (int round = 9; round >= 0; --round) {
            for (size_t i = decrypted.length(); i > 0; --i) {
                size_t idx = i - 1;
                uint8_t byte = static_cast<uint8_t>(decrypted[idx]);
                uint8_t keyByte = static_cast<uint8_t>(key[idx % key.length()]);
                
                // Reverse MixColumns
                if (idx > 0) {
                    byte ^= static_cast<uint8_t>(decrypted[idx-1]);
                }
                
                // Reverse SubBytes
                byte = ((byte ^ (keyByte << 3)) - (byte << 1)) ^ keyByte;
                
                decrypted[idx] = static_cast<char>(byte);
            }
        }
        
        // Remove padding
        if (!decrypted.empty()) {
            size_t padding = static_cast<uint8_t>(decrypted.back());
            if (padding <= 16 && padding > 0 && decrypted.length() >= padding) {
                decrypted = decrypted.substr(0, decrypted.length() - padding);
            }
        }
        
        return decrypted;
    }
    
    // ============= HMAC Implementation =============
    
    std::string IPSecManager::computeHMAC(const std::string& data, const std::string& key,
                                          AuthenticationAlgorithm algo) const {
        std::string hashOutput;
        
        switch (algo) {
            case AuthenticationAlgorithm::HMAC_SHA256:
                hashOutput = sha256(key + data + key);
                break;
            case AuthenticationAlgorithm::HMAC_SHA384:
            case AuthenticationAlgorithm::HMAC_SHA512:
                hashOutput = sha512(key + data + key);
                break;
            default:
                hashOutput = sha256(key + data + key);
        }
        
        return hashOutput;
    }
    
    bool IPSecManager::verifyHMAC(const std::string& data, 
                                  const std::string& signature, 
                                  const std::string& key,
                                  AuthenticationAlgorithm algo) const {
        std::string calculated = computeHMAC(data, key, algo);
        return calculated == signature;
    }
    
    // ============= Hash Functions =============
    
    std::string IPSecManager::sha256(const std::string& data) const {
        // Simulated SHA-256 using multiple rounds of hashing
        std::hash<std::string> hasher;
        size_t hash1 = hasher(data);
        size_t hash2 = hasher(data + "salt1");
        size_t hash3 = hasher("salt2" + data);
        size_t hash4 = hasher(data + "salt3");
        
        // Combine hashes to create a 256-bit equivalent output
        std::ostringstream oss;
        oss << std::hex << std::setfill('0') << std::setw(16) << hash1
            << std::setw(16) << hash2 << std::setw(16) << hash3 << std::setw(16) << hash4;
        
        std::string result = oss.str();
        return result.substr(0, 64);  // 256 bits = 64 hex characters
    }
    
    std::string IPSecManager::sha512(const std::string& data) const {
        // Simulated SHA-512 using multiple rounds of hashing
        std::hash<std::string> hasher;
        size_t hash1 = hasher(data);
        size_t hash2 = hasher(data + "salt1");
        size_t hash3 = hasher("salt2" + data);
        size_t hash4 = hasher(data + "salt3");
        size_t hash5 = hasher(data + "salt4");
        size_t hash6 = hasher("salt5" + data);
        size_t hash7 = hasher(data + "salt6");
        size_t hash8 = hasher("salt7" + data);
        
        // Combine hashes to create a 512-bit equivalent output
        std::ostringstream oss;
        oss << std::hex << std::setfill('0') << std::setw(16) << hash1
            << std::setw(16) << hash2 << std::setw(16) << hash3 << std::setw(16) << hash4
            << std::setw(16) << hash5 << std::setw(16) << hash6 << std::setw(16) << hash7
            << std::setw(16) << hash8;
        
        std::string result = oss.str();
        return result.substr(0, 128);  // 512 bits = 128 hex characters
    }
    
} // namespace iot