#include <iostream>
#include <memory>
#include "../include/security/IPSecManager.h"

int main() {
    std::cout << "=========================================" << std::endl;
    std::cout << "IPsec Security Layer Test" << std::endl;
    std::cout << "=========================================" << std::endl;
    
    try {
        // Create IPsec manager
        auto ipsecManager = std::make_shared<iot::IPSecManager>(iot::IPSecManager::IPsecMode::TRANSPORT);
        
        std::cout << "\n1. Testing Security Association Creation..." << std::endl;
        
        // Create security associations
        bool sa1 = ipsecManager->createSecurityAssociation("192.168.1.10", "192.168.1.20");
        bool sa2 = ipsecManager->createSecurityAssociation("192.168.1.30", "192.168.1.40", "CUSTOM_SPI_12345");
        
        std::cout << "SA Creation Results: " << (sa1 ? "SUCCESS" : "FAILED") 
                  << ", " << (sa2 ? "SUCCESS" : "FAILED") << std::endl;
        
        std::cout << "\n2. Testing Security Policies..." << std::endl;
        
        // Add security policies
        iot::SecurityPolicy policy1;
        policy1.sourceIP = "192.168.1.10";
        policy1.destinationIP = "192.168.1.20";
        policy1.protocol = "ESP";
        policy1.requireEncryption = true;
        policy1.requireAuthentication = true;
        policy1.securityLevel = 8;
        
        bool policyAdded = ipsecManager->addSecurityPolicy("192.168.1.10", "192.168.1.20", policy1);
        std::cout << "Policy Addition: " << (policyAdded ? "SUCCESS" : "FAILED") << std::endl;
        
        std::cout << "\n3. Testing Communication Security Check..." << std::endl;
        
        // Check if communication should be secured
        bool shouldSecure1 = ipsecManager->shouldSecureCommunication("192.168.1.10", "192.168.1.20");
        bool shouldSecure2 = ipsecManager->shouldSecureCommunication("192.168.1.50", "192.168.1.50");  // Same device
        
        std::cout << "Should secure 10->20: " << (shouldSecure1 ? "YES" : "NO") << std::endl;
        std::cout << "Should secure 50->50: " << (shouldSecure2 ? "YES" : "NO") << std::endl;
        
        std::cout << "\n4. Testing ESP Encryption and Authentication..." << std::endl;
        
        // Test encryption and authentication
        std::string originalMessage = "Secret IoT  Temperature=23.5C,Humidity=45%";
        std::string securedMessage = ipsecManager->encryptAndAuthenticate(
            originalMessage, "192.168.1.10", "192.168.1.20");
        
        std::cout << "Original: " << originalMessage << std::endl;
        std::cout << "Secured: " << securedMessage << std::endl;
        
        std::cout << "\n5. Testing ESP Decryption and Verification..." << std::endl;
        
        // Test decryption and verification
        std::string decryptedMessage = ipsecManager->decryptAndVerify(
            securedMessage, "192.168.1.10", "192.168.1.20");
        
        std::cout << "Decrypted: " << decryptedMessage << std::endl;
        std::cout << "Match: " << (originalMessage == decryptedMessage ? "YES" : "NO") << std::endl;
        
        std::cout << "\n6. Testing AH Authentication Only..." << std::endl;
        
        // Test authentication only (AH)
        std::string authMessage = ipsecManager->authenticateOnly(
            originalMessage, "192.168.1.30", "192.168.1.40");
        
        std::cout << "Authenticated: " << authMessage << std::endl;
        
        std::cout << "\n7. IPsec Statistics..." << std::endl;
        ipsecManager->printIPSecStatistics();
        
        std::cout << "\n=========================================" << std::endl;
        std::cout << "IPsec Security Layer Test COMPLETED!" << std::endl;
        std::cout << "=========================================" << std::endl;
        std::cout << "Key Features Demonstrated:" << std::endl;
        std::cout << "• Security Association Management" << std::endl;
        std::cout << "• Security Policy Enforcement" << std::endl;
        std::cout << "• ESP Encryption and Authentication" << std::endl;
        std::cout << "• AH Authentication Only Mode" << std::endl;
        std::cout << "• SPI Generation and Management" << std::endl;
        std::cout << "• Key Generation for Cryptography" << std::endl;
        std::cout << "=========================================" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error in IPsec test: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}