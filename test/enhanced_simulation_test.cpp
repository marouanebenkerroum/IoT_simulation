#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <thread>
#include <random>

// Core System Headers
#include "../include/core/DeviceManager.h"
#include "../include/core/IoTDevice.h"
#include "../include/network/NetworkManager.h"
#include "../include/simulation/SimulationEngine.h"
#include "../include/security/SecurityManager.h"
#include "../include/security/IPSecManager.h"

// Device Headers
#include "../include/devices/ConcreteSensors.h"
#include "../include/devices/ConcreteActuators.h"
#include "../include/devices/NetworkMonitor.h"

// Platform-specific headers for resource usage (Linux/WSL)
#ifdef __linux__
#include <sys/resource.h>
#endif

/**
 * @brief Gets the peak resident set size (max memory usage) in kilobytes.
 * This is a Linux/WSL-specific function.
 * @return Peak memory usage in KB, or -1 on failure.
 */
long getPeakMemoryUsageKB() {
#ifdef __linux__
    std::ifstream status_file("/proc/self/status");
    if (!status_file.is_open()) {
        std::cerr << "Warning: Could not open /proc/self/status to read memory." << std::endl;
        return -1;
    }

    std::string line;
    while (std::getline(status_file, line)) {
        if (line.rfind("VmPeak:", 0) == 0) {
            std::istringstream iss(line);
            std::string key;
            long value;
            std::string unit;
            iss >> key >> value >> unit;
            if (unit == "kB") {
                return value;
            }
            return -1; // Found VmPeak but unit was not kB
        }
    }
#endif
    return -1; // Not available or VmPeak not found
}

/**
 * @brief Gets the total (user + system) CPU time used by this process.
 * @return Total CPU time in seconds.
 */
double getTotalCpuTime() {
#ifdef __linux__
    struct rusage usage;
    if (getrusage(RUSAGE_SELF, &usage) == 0) {
        double user_time = static_cast<double>(usage.ru_utime.tv_sec) + static_cast<double>(usage.ru_utime.tv_usec) / 1e6;
        double sys_time = static_cast<double>(usage.ru_stime.tv_sec) + static_cast<double>(usage.ru_stime.tv_usec) / 1e6;
        return user_time + sys_time;
    }
#endif
    std::cerr << "Warning: Could not get CPU time." << std::endl;
    return -1.0;
}

/**
 * @brief Prints the results in a clean, parseable key-value format.
 */
void printResults(int num_devices, bool security_enabled, double setup_ms, double run_ms, double total_wall_s, double total_cpu_s, long peak_mem_kb) {
    std::cout << "\n=== SCALABILITY TEST RESULTS ===" << std::endl;
    std::cout << std::left << std::setw(25) << "Metric" << "Value" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    std::cout << std::left << std::setw(25) << "DeviceCount" << num_devices << std::endl;
    std::cout << std::left << std::setw(25) << "SecurityEnabled" << (security_enabled ? "true" : "false") << std::endl;
    std::cout << std::left << std::setw(25) << "StartupTime_ms" << std::fixed << std::setprecision(2) << setup_ms << std::endl;
    std::cout << std::left << std::setw(25) << "SimulationRunTime_ms" << std::fixed << std::setprecision(2) << run_ms << std::endl;
    std::cout << std::left << std::setw(25) << "TotalWallTime_s" << std::fixed << std::setprecision(2) << total_wall_s << std::endl;
    std::cout << std::left << std::setw(25) << "TotalCpuTime_s" << std::fixed << std::setprecision(2) << total_cpu_s << std::endl;
    std::cout << std::left << std::setw(25) << "PeakMemory_kB" << peak_mem_kb << std::endl;
    std::cout << "========================================" << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: ./scalability_test <num_devices> <security_enabled (0 or 1)>" << std::endl;
        std::cerr << "Example: ./scalability_test 1000 0" << std::endl;
        return 1;
    }

    int num_devices = 0;
    bool security_enabled = false;
    try {
        num_devices = std::stoi(argv[1]);
        security_enabled = (std::stoi(argv[2]) != 0);
    } catch (const std::exception& e) {
        std::cerr << "Error: Invalid arguments. " << e.what() << std::endl;
        return 1;
    }

    if (num_devices <= 0) {
        std::cerr << "Error: num_devices must be greater than 0." << std::endl;
        return 1;
    }

    std::cout << "=========================================" << std::endl;
    std::cout << "IoT Network Simulation Scalability Test" << std::endl;
    std::cout << "=========================================" << std::endl;
    std::cout << "Devices: " << num_devices << std::endl;
    std::cout << "Security: " << (security_enabled ? "ENABLED" : "DISABLED") << std::endl;
    std::cout << "=========================================" << std::endl;

    // --- Start Total Test Timing ---
    auto test_start_wall_time = std::chrono::high_resolution_clock::now();
    double start_cpu_time = getTotalCpuTime();

    try {
        // 1. Initialize Core Managers
        std::cout << "\n1. Initializing Core Components..." << std::endl;
        auto deviceManager = std::make_shared<iot::DeviceManager>();
        auto networkManager = std::make_shared<iot::NetworkManager>(deviceManager);
        auto securityManager = std::make_shared<iot::SecurityManager>(iot::SecurityManager::SecurityLevel::ENHANCED);
        auto ipsecManager = std::make_shared<iot::IPSecManager>(iot::IPSecManager::IPsecMode::TRANSPORT);
        auto simulationEngine = std::make_shared<iot::SimulationEngine>(deviceManager, networkManager);

        std::cout << "✓ Core components initialized" << std::endl;

        // 2. Configure Security (if enabled)
        if (security_enabled) {
            std::cout << "\n2. Configuring Security..." << std::endl;
            networkManager->setIPSecManager(ipsecManager);
            std::cout << "✓ Security configured" << std::endl;
        }

        // 3. Start Network Manager
        networkManager->start();
        std::cout << "✓ Network manager started" << std::endl;

        // --- Start Startup Phase Timing ---
        std::cout << "\n3. Starting Device Registration (" << num_devices << " devices)..." << std::endl;
        auto setup_start_time = std::chrono::high_resolution_clock::now();

        // 4. Create and Register Devices
        std::vector<std::shared_ptr<iot::IoTDevice>> devices;
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 1);

        // Register a central monitor to receive sensor/actuator messages
        auto monitor = std::make_shared<iot::NetworkMonitor>("NETWORK_MONITOR", "Network Monitor");
        deviceManager->registerDevice(monitor);

        for (int i = 0; i < num_devices; ++i) {
            // Create a mix of devices: random sensor/actuator types
            if (dis(gen) == 0) {
                // Create sensors
                int sensor_type = i % 3;
                switch (sensor_type) {
                    case 0: {
                        std::string id = "TEMP_" + std::to_string(i);
                        auto sensor = std::make_shared<iot::TemperatureSensor>(id, "Temperature Sensor " + std::to_string(i));
                        deviceManager->registerDevice(sensor);
                        devices.push_back(sensor);
                        break;
                    }
                    case 1: {
                        std::string id = "HUM_" + std::to_string(i);
                        auto sensor = std::make_shared<iot::HumiditySensor>(id, "Humidity Sensor " + std::to_string(i));
                        deviceManager->registerDevice(sensor);
                        devices.push_back(sensor);
                        break;
                    }
                    case 2: {
                        std::string id = "MOTION_" + std::to_string(i);
                        auto sensor = std::make_shared<iot::MotionSensor>(id, "Motion Sensor " + std::to_string(i));
                        deviceManager->registerDevice(sensor);
                        devices.push_back(sensor);
                        break;
                    }
                }
            } else {
                // Create actuators
                int actuator_type = i % 3;
                switch (actuator_type) {
                    case 0: {
                        std::string id = "LED_" + std::to_string(i);
                        auto actuator = std::make_shared<iot::LED>(id, "LED Actuator " + std::to_string(i));
                        deviceManager->registerDevice(actuator);
                        devices.push_back(actuator);
                        break;
                    }
                    case 1: {
                        std::string id = "MOTOR_" + std::to_string(i);
                        auto actuator = std::make_shared<iot::Motor>(id, "Motor Actuator " + std::to_string(i), 100);
                        deviceManager->registerDevice(actuator);
                        devices.push_back(actuator);
                        break;
                    }
                    case 2: {
                        std::string id = "RELAY_" + std::to_string(i);
                        auto actuator = std::make_shared<iot::Relay>(id, "Relay Actuator " + std::to_string(i), 15.0);
                        deviceManager->registerDevice(actuator);
                        devices.push_back(actuator);
                        break;
                    }
                }
            }
        }

        // --- End Startup Phase Timing ---
        auto setup_end_time = std::chrono::high_resolution_clock::now();
        std::cout << "✓ Device registration completed (" << devices.size() << " devices)" << std::endl;

// 5. Schedule Simulation Events
std::cout << "\n4. Scheduling Simulation Events..." << std::endl;

// Schedule events that actually send messages through the network
for (const auto& device : devices) {
    // Temperature sensor events - send actual messages
    if (auto tempSensor = std::dynamic_pointer_cast<iot::TemperatureSensor>(device)) {
        simulationEngine->scheduleRepeatingEvent(
            std::chrono::milliseconds(3000),
            [tempSensor, networkManager]() {
                double temp = tempSensor->readValue();
                std::cout << "[SENSOR] Temperature: " << std::fixed << std::setprecision(1) 
                         << temp << "°C from " << tempSensor->getDeviceId() << std::endl;
                
                // ACTUALLY SEND MESSAGE THROUGH NETWORK MANAGER
                iot::Message tempMsg(tempSensor->getDeviceId(), "NETWORK_MONITOR", 
                                   "TEMP:" + std::to_string(temp));
                networkManager->sendMessage(tempMsg);
            },
            "TEMP_READING_" + device->getDeviceId()
        );
    }
    // Humidity sensor events
    else if (auto humSensor = std::dynamic_pointer_cast<iot::HumiditySensor>(device)) {
        simulationEngine->scheduleRepeatingEvent(
            std::chrono::milliseconds(4000),
            [humSensor, networkManager]() {
                double humidity = humSensor->readValue();
                std::cout << "[SENSOR] Humidity: " << std::fixed << std::setprecision(1) 
                         << humidity << "% from " << humSensor->getDeviceId() << std::endl;
                
                // ACTUALLY SEND MESSAGE THROUGH NETWORK MANAGER
                iot::Message humMsg(humSensor->getDeviceId(), "NETWORK_MONITOR", 
                                  "HUMIDITY:" + std::to_string(humidity));
                networkManager->sendMessage(humMsg);
            },
            "HUMIDITY_READING_" + device->getDeviceId()
        );
    }
    // Motion sensor events
    else if (auto motionSensor = std::dynamic_pointer_cast<iot::MotionSensor>(device)) {
        simulationEngine->scheduleRepeatingEvent(
            std::chrono::milliseconds(5000),
            [motionSensor, networkManager]() {
                double motion = motionSensor->readValue();
                if (motion > 0.5) {
                    std::cout << "[SENSOR] MOTION DETECTED from " << motionSensor->getDeviceId() << std::endl;
                    
                    // ACTUALLY SEND MESSAGE THROUGH NETWORK MANAGER
                    iot::Message motionMsg(motionSensor->getDeviceId(), "NETWORK_MONITOR", "MOTION_ALERT");
                    networkManager->sendMessage(motionMsg);
                }
            },
            "MOTION_DETECTION_" + device->getDeviceId()
        );
    }
    // Actuator events
    else if (auto actuator = std::dynamic_pointer_cast<iot::Actuator>(device)) {
        simulationEngine->scheduleRepeatingEvent(
            std::chrono::milliseconds(10000),
            [actuator, networkManager]() {
                actuator->toggle();
                std::cout << "[ACTUATOR] " << actuator->getDeviceId() << " toggled" << std::endl;
                
                // ACTUALLY SEND MESSAGE THROUGH NETWORK MANAGER
                iot::Message actuatorMsg(actuator->getDeviceId(), "NETWORK_MONITOR", "TOGGLED");
                networkManager->sendMessage(actuatorMsg);
            },
            "ACTUATOR_TOGGLE_" + device->getDeviceId()
        );
    }
}
        // 6. Run Simulation
        std::cout << "\n5. Running Simulation for 30 seconds..." << std::endl;
        auto run_start_time = std::chrono::high_resolution_clock::now();
        simulationEngine->start();
        
        // Run for 30 seconds of simulated time
        std::this_thread::sleep_for(std::chrono::seconds(30));
        
        simulationEngine->stop();
        auto run_end_time = std::chrono::high_resolution_clock::now();
        std::cout << "✓ Simulation completed" << std::endl;

        // 7. Print Statistics
        std::cout << "\n6. Collecting Performance Statistics..." << std::endl;
        deviceManager->printStats();
        networkManager->printStats();
        simulationEngine->printStats();

        // --- End Total Test Timing ---
        auto test_end_wall_time = std::chrono::high_resolution_clock::now();
        double end_cpu_time = getTotalCpuTime();
        long peak_memory_kb = getPeakMemoryUsageKB();

        // 8. Calculate and Print Metrics
        double setup_duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(setup_end_time - setup_start_time).count();
        double run_duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(run_end_time - run_start_time).count();
        double total_wall_time_s = std::chrono::duration_cast<std::chrono::duration<double>>(test_end_wall_time - test_start_wall_time).count();
        double total_cpu_time_s = (end_cpu_time >= 0 && start_cpu_time >= 0) ? end_cpu_time - start_cpu_time : -1.0;

        printResults(num_devices, security_enabled, setup_duration_ms, run_duration_ms, total_wall_time_s, total_cpu_time_s, peak_memory_kb);

        // Clean up
        networkManager->stop();
        devices.clear();

        std::cout << "\nScalability test completed successfully!" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Error in scalability test: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}