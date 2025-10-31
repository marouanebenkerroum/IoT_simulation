#include "../../include/simulation/SimulationEngine.h"
#include <iostream>
#include <algorithm>
#include <thread>
#include <chrono>

namespace iot {
    
    SimulationEngine::SimulationEngine(std::shared_ptr<DeviceManager> dm, 
                                     std::shared_ptr<NetworkManager> nm)
        : deviceManager(dm)
        , networkManager(nm)
        , currentState(State::STOPPED)
        , simulationTimeStep(100)  // 100ms default time step
        , simulationSpeed(1.0)
        , running(false)
        , config{1.0, 1000, 0.0, 0.0, 0.0, "INFO", "simulation.log"}
        , totalEventsProcessed(0)
        , simulationSteps(0) {
        std::cout << "Simulation Engine initialized" << std::endl;
    }
    
    SimulationEngine::~SimulationEngine() {
        stop();
        std::cout << "Simulation Engine destroyed" << std::endl;
    }
    
    void SimulationEngine::start() {
        std::lock_guard<std::mutex> lock(stateMutex);
        
        if (currentState != State::STOPPED) {
            std::cout << "Simulation is already running or paused" << std::endl;
            return;
        }
        
        std::cout << "Starting simulation engine..." << std::endl;
        
        currentState = State::RUNNING;
        running = true;
        startTime = std::chrono::steady_clock::now();
        currentTime = startTime;
        
        // Start network manager if not already started
        if (networkManager) {
            networkManager->start();
        }
        
        // Start simulation thread
        simulationThread = std::thread(&SimulationEngine::runSimulation, this);
        
        std::cout << "Simulation engine started" << std::endl;
    }
    
    void SimulationEngine::stop() {
        {
            std::lock_guard<std::mutex> lock(stateMutex);
            if (currentState == State::STOPPED) return;
            
            std::cout << "Stopping simulation engine..." << std::endl;
            running = false;
            currentState = State::STOPPED;
        }
        
        eventCondition.notify_all();
        
        if (simulationThread.joinable()) {
            simulationThread.join();
        }
        
        // Print stats before stopping network to ensure visibility even on quick shutdown
        if (networkManager) {
            networkManager->printStats();
            networkManager->stop();
        }
        
        std::cout << "Simulation engine stopped" << std::endl;
    }
    
    void SimulationEngine::pause() {
        std::lock_guard<std::mutex> lock(stateMutex);
        if (currentState == State::RUNNING) {
            currentState = State::PAUSED;
            std::cout << "Simulation paused" << std::endl;
        }
    }
    
    void SimulationEngine::resume() {
        std::lock_guard<std::mutex> lock(stateMutex);
        if (currentState == State::PAUSED) {
            currentState = State::RUNNING;
            std::cout << "Simulation resumed" << std::endl;
        }
    }
    
    SimulationEngine::State SimulationEngine::getState() const {
        std::lock_guard<std::mutex> lock(stateMutex);
        return currentState;
    }
    
    void SimulationEngine::scheduleEvent(const std::chrono::milliseconds& delay,
                                       std::function<void()> callback,
                                       const std::string& eventId,
                                       int priority) {
        auto scheduledTime = std::chrono::steady_clock::now() + delay;
        
        SimulationEvent event;
        event.scheduledTime = scheduledTime;
        event.eventId = eventId.empty() ? "EVENT_" + std::to_string(totalEventsProcessed) : eventId;
        event.callback = callback;
        event.priority = priority;
        
        {
            std::lock_guard<std::mutex> lock(eventMutex);
            eventQueue.push(event);
        }
        
        eventCondition.notify_one();
        
        std::cout << "Event scheduled: " << event.eventId 
                  << " at " << scheduledTime.time_since_epoch().count() << std::endl;
    }
    
    void SimulationEngine::scheduleRepeatingEvent(const std::chrono::milliseconds& interval,
                                                std::function<void()> callback,
                                                const std::string& eventId,
                                                int priority) {
        // Create a wrapper that reschedules itself
        std::string actualEventId = eventId.empty() ? "REPEAT_" + std::to_string(totalEventsProcessed) : eventId;
        
        std::function<void()> repeatingCallback = [this, interval, callback, actualEventId, priority]() {
            callback();  // Execute the original callback
            // Reschedule the event
            scheduleEvent(interval, 
                         [this, interval, callback, actualEventId, priority]() {
                             scheduleRepeatingEvent(interval, callback, actualEventId, priority);
                         }, 
                         actualEventId, priority);
        };
        
        scheduleEvent(interval, repeatingCallback, actualEventId, priority);
    }
    
    void SimulationEngine::setSimulationSpeed(double speed) {
        simulationSpeed = std::max(0.01, speed);  // Minimum 1% speed
        std::cout << "Simulation speed set to " << simulationSpeed << "x" << std::endl;
    }
    
    std::chrono::steady_clock::time_point SimulationEngine::getCurrentTime() const {
        std::lock_guard<std::mutex> lock(stateMutex);
        return currentTime;
    }
    
    bool SimulationEngine::loadConfig(const std::string& configFile) {
    // Create a simple config string for demonstration
    std::string configString = R"(
simulation.speed=1.5
network.packet_loss=0.02
network.delay_min=50.0
network.delay_max=200.0
logging.level=DEBUG
max_devices=500
)";
    
    ConfigManager configMgr;
    if (configMgr.loadFromString(configString)) {
        std::cout << "Configuration loaded from string" << std::endl;
        
        // Apply configuration
        config.simulationSpeed = configMgr.getDouble("simulation.speed", 1.0);
        config.packetLossRate = configMgr.getDouble("network.packet_loss", 0.0);
        config.networkDelayMin = configMgr.getDouble("network.delay_min", 0.0);
        config.networkDelayMax = configMgr.getDouble("network.delay_max", 0.0);
        config.logLevel = configMgr.getString("logging.level", "INFO");
        config.maxDevices = configMgr.getInt("max_devices", 1000);
        
        // Apply network configuration
        if (networkManager) {
            networkManager->setNetworkConditions(
                config.packetLossRate,
                config.networkDelayMin,
                config.networkDelayMax
            );
        }
        
        setSimulationSpeed(config.simulationSpeed);
        
        std::cout << "Applied configuration:" << std::endl;
        std::cout << "  Simulation Speed: " << config.simulationSpeed << "x" << std::endl;
        std::cout << "  Packet Loss: " << config.packetLossRate << std::endl;
        std::cout << "  Network Delay: " << config.networkDelayMin << "-" << config.networkDelayMax << "ms" << std::endl;
        std::cout << "  Log Level: " << config.logLevel << std::endl;
        std::cout << "  Max Devices: " << config.maxDevices << std::endl;
        
        return true;
    }
    
    std::cout << "Failed to load configuration" << std::endl;
    return false;
}
    
    void SimulationEngine::printStats() const {
        std::cout << "\n=== Simulation Statistics ===" << std::endl;
        std::cout << "Total Events Processed: " << totalEventsProcessed << std::endl;
        std::cout << "Simulation Steps: " << simulationSteps << std::endl;
        std::cout << "Current State: ";
        switch (getState()) {
            case State::RUNNING: std::cout << "RUNNING"; break;
            case State::PAUSED: std::cout << "PAUSED"; break;
            case State::STOPPED: std::cout << "STOPPED"; break;
        }
        std::cout << std::endl;
        std::cout << "Simulation Speed: " << simulationSpeed << "x" << std::endl;
        
        if (networkManager) {
            networkManager->printStats();
        }
        std::cout << "=============================" << std::endl;
    }
    
    void SimulationEngine::runSimulation() {
        std::cout << "Simulation loop started" << std::endl;
        
        while (running) {
            {
                std::lock_guard<std::mutex> lock(stateMutex);
                if (currentState != State::RUNNING) {
                    // Wait a bit if paused or stopped
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                    continue;
                }
            }
            
            // Execute simulation step
            simulationStep();
            
            // Process scheduled events
            processEvents();
            
            // Apply simulation speed
            if (simulationSpeed != 1.0) {
                auto sleepTime = std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::duration<double, std::milli>(simulationTimeStep.count() / simulationSpeed)
                );
                std::this_thread::sleep_for(sleepTime);
            } else {
                std::this_thread::sleep_for(simulationTimeStep);
            }
        }
        
        std::cout << "Simulation loop ended" << std::endl;
    }
    
    void SimulationEngine::processEvents() {
        auto now = std::chrono::steady_clock::now();
        
        std::unique_lock<std::mutex> lock(eventMutex);
        while (!eventQueue.empty() && eventQueue.top().scheduledTime <= now) {
            SimulationEvent event = eventQueue.top();
            eventQueue.pop();
            
            lock.unlock();  // Unlock before executing callback
            
            try {
                if (event.callback) {
                    event.callback();
                    totalEventsProcessed++;
                }
            } catch (const std::exception& e) {
                std::cerr << "Error executing event " << event.eventId << ": " << e.what() << std::endl;
            }
            
            lock.lock();  // Lock again for next iteration
        }
    }
    
    void SimulationEngine::simulationStep() {
        simulationSteps++;
        currentTime = std::chrono::steady_clock::now();
        
        // This is where you'd add periodic simulation logic
        // For example, you could trigger device updates, network checks, etc.
        
        if (simulationSteps % 100 == 0) {  // Every 100 steps
            // Periodic maintenance or logging
        }
    }
    
} // namespace iot