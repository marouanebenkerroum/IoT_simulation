#include "../../include/utils/ConfigManager.h"
#include <iostream>
#include <sstream>
#include <algorithm>

namespace iot {
    
    ConfigManager::ConfigManager() {
        // Default configuration values
        set("simulation.speed", "1.0");
        set("network.packet_loss", "0.0");
        set("network.delay_min", "0.0");
        set("network.delay_max", "0.0");
        set("logging.level", "INFO");
    }
    
    bool ConfigManager::loadFromString(const std::string& configString) {
        // Simple key=value parser (in reality, you'd parse JSON/YAML)
        std::istringstream iss(configString);
        std::string line;
        
        while (std::getline(iss, line)) {
            // Skip empty lines and comments
            if (line.empty() || line[0] == '#' || line[0] == ';') continue;
            
            // Find '=' separator
            size_t eqPos = line.find('=');
            if (eqPos != std::string::npos) {
                std::string key = line.substr(0, eqPos);
                std::string value = line.substr(eqPos + 1);
                
                // Trim whitespace
                key.erase(0, key.find_first_not_of(" \t"));
                key.erase(key.find_last_not_of(" \t") + 1);
                value.erase(0, value.find_first_not_of(" \t"));
                value.erase(value.find_last_not_of(" \t") + 1);
                
                set(key, value);
            }
        }
        
        return true;
    }
    
    std::string ConfigManager::getString(const std::string& key, const std::string& defaultValue) const {
        auto it = configValues.find(key);
        if (it != configValues.end()) {
            return it->second;
        }
        return defaultValue;
    }
    
    int ConfigManager::getInt(const std::string& key, int defaultValue) const {
        auto it = configValues.find(key);
        if (it != configValues.end()) {
            try {
                return std::stoi(it->second);
            } catch (...) {
                return defaultValue;
            }
        }
        return defaultValue;
    }
    
    double ConfigManager::getDouble(const std::string& key, double defaultValue) const {
        auto it = configValues.find(key);
        if (it != configValues.end()) {
            try {
                return std::stod(it->second);
            } catch (...) {
                return defaultValue;
            }
        }
        return defaultValue;
    }
    
    bool ConfigManager::getBool(const std::string& key, bool defaultValue) const {
        auto it = configValues.find(key);
        if (it != configValues.end()) {
            std::string value = it->second;
            std::transform(value.begin(), value.end(), value.begin(), ::tolower);
            return (value == "true" || value == "1" || value == "yes" || value == "on");
        }
        return defaultValue;
    }
    
    void ConfigManager::set(const std::string& key, const std::string& value) {
        configValues[key] = value;
    }
    
    bool ConfigManager::hasKey(const std::string& key) const {
        return configValues.find(key) != configValues.end();
    }
    
} // namespace iot