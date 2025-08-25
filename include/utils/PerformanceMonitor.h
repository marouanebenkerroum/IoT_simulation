#ifndef IOT_SIMULATION_PERFORMANCE_MONITOR_H
#define IOT_SIMULATION_PERFORMANCE_MONITOR_H

#include <chrono>
#include <iostream>
#include <map>
#include <string>

namespace iot {
    
    /**
     * @brief Performance monitoring and profiling utility
     */
    class PerformanceMonitor {
    private:
        struct Metric {
            size_t count;
            double totalTime;
            double minTime;
            double maxTime;
        };
        
        std::map<std::string, Metric> metrics;
        std::chrono::steady_clock::time_point startTime;
        
    public:
        /**
         * @brief Constructor
         */
        PerformanceMonitor();
        
        /**
         * @brief Start timing a specific operation
         */
        void startOperation(const std::string& operationName);
        
        /**
         * @brief End timing an operation
         */
        void endOperation(const std::string& operationName);
        
        /**
         * @brief Record a specific timing measurement
         */
        void recordTime(const std::string& operationName, double milliseconds);
        
        /**
         * @brief Get average time for an operation
         */
        double getAverageTime(const std::string& operationName) const;
        
        /**
         * @brief Print performance report
         */
        void printReport() const;
        
        /**
         * @brief Reset all metrics
         */
        void reset();
    };
    
} // namespace iot

#endif // IOT_SIMULATION_PERFORMANCE_MONITOR_H