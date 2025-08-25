#include "../../include/utils/PerformanceMonitor.h"
#include <algorithm>
#include <limits>

namespace iot {
    
    PerformanceMonitor::PerformanceMonitor() 
        : startTime(std::chrono::steady_clock::now()) {
    }
    
    void PerformanceMonitor::startOperation(const std::string& operationName) {
        // In a real implementation, you'd track start times
        // For now, we'll use the recordTime approach
    }
    
    void PerformanceMonitor::endOperation(const std::string& operationName) {
        // In a real implementation, you'd calculate elapsed time
    }
    
    void PerformanceMonitor::recordTime(const std::string& operationName, double milliseconds) {
        auto& metric = metrics[operationName];
        metric.count++;
        metric.totalTime += milliseconds;
        metric.minTime = (metric.count == 1) ? milliseconds : std::min(metric.minTime, milliseconds);
        metric.maxTime = (metric.count == 1) ? milliseconds : std::max(metric.maxTime, milliseconds);
    }
    
    double PerformanceMonitor::getAverageTime(const std::string& operationName) const {
        auto it = metrics.find(operationName);
        if (it != metrics.end() && it->second.count > 0) {
            return it->second.totalTime / it->second.count;
        }
        return 0.0;
    }
    
    void PerformanceMonitor::printReport() const {
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - startTime);
        
        std::cout << "\n=== PERFORMANCE MONITOR REPORT ===" << std::endl;
        std::cout << "Total Runtime: " << duration.count() << " ms" << std::endl;
        std::cout << "Monitored Operations:" << std::endl;
        
        for (const auto& pair : metrics) {
            const std::string& name = pair.first;
            const Metric& metric = pair.second;
            
            std::cout << "  " << name << ":" << std::endl;
            std::cout << "    Count: " << metric.count << std::endl;
            std::cout << "    Average: " << (metric.totalTime / metric.count) << " ms" << std::endl;
            std::cout << "    Min: " << metric.minTime << " ms" << std::endl;
            std::cout << "    Max: " << metric.maxTime << " ms" << std::endl;
            std::cout << "    Total: " << metric.totalTime << " ms" << std::endl;
        }
        std::cout << "=================================" << std::endl;
    }
    
    void PerformanceMonitor::reset() {
        metrics.clear();
        startTime = std::chrono::steady_clock::now();
    }
    
} // namespace iot