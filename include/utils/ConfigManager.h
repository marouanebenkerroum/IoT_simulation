#ifndef IOT_SIMULATION_CONFIG_MANAGER_H
#define IOT_SIMULATION_CONFIG_MANAGER_H

#include <string>
#include <map>
#include <vector>

namespace iot {
    
    /**
     * @brief Simple configuration manager
     */
    class ConfigManager {
    private:
        std::map<std::string, std::string> configValues;
        
    public:
        /**
         * @brief Constructor
         */
        ConfigManager();
        
        /**
         * @brief Load configuration from string (simplified)
         */
        bool loadFromString(const std::string& configString);
        
        /**
         * @brief Get string value
         */
        std::string getString(const std::string& key, const std::string& defaultValue = "") const;
        
        /**
         * @brief Get integer value
         */
        int getInt(const std::string& key, int defaultValue = 0) const;
        
        /**
         * @brief Get double value
         */
        double getDouble(const std::string& key, double defaultValue = 0.0) const;
        
        /**
         * @brief Get boolean value
         */
        bool getBool(const std::string& key, bool defaultValue = false) const;
        
        /**
         * @brief Set value
         */
        void set(const std::string& key, const std::string& value);
        
        /**
         * @brief Check if key exists
         */
        bool hasKey(const std::string& key) const;
    };
    
} // namespace iot

#endif // IOT_SIMULATION_CONFIG_MANAGER_H