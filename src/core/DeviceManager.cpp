#include "../../include/core/DeviceManager.h"
#include "../../include/core/Message.h"
#include <iostream>
#include <algorithm>
#include <random>

namespace iot
{
    DeviceManager::DeviceManager()
        : nextId(1){}

    bool DeviceManager::registerDevice(std::shared_ptr<IoTDevice> device){
        if(!device){
            std::cerr << "Error: Cannot register Null device" << std::endl;
            return false;
        }

        std::lock_guard<std::mutex> lock(devicesMutex);
        std::string deviceId = device->getDeviceId();

        if(devices.find(deviceId) != devices.end()){
            std::cerr << "Error : Device With ID" << deviceId << "already exists" << std::endl;
            return false;
        }

        devices[deviceId] = device;
        deviceIds.push_back(deviceId);
        std::cout << "Device registred: " << deviceId << std::endl;
        return true;
    }

    bool DeviceManager::unregisterDevice(const std::string& deviceId){
        auto it = devices.find(deviceId);
        if(it == devices.end()){
            std::cerr << "Error: Device" << deviceId << "not found" << std::endl;
            return false;
        }

        devices.erase(it);
        deviceIds.erase(std::remove(deviceIds.begin(), deviceIds.end(), deviceId), deviceIds.end());
        std::cout << "Device unregistred: " << deviceId << std::endl;
        return true;
    }

    std::shared_ptr<IoTDevice> DeviceManager::getDevice(const std::string& deviceId) const{
        std::lock_guard<std::mutex> lock(devicesMutex);

        auto it = devices.find(deviceId);
        if (it != devices.end()){
            return it->second;
        }
        return nullptr;
    }

    std::vector<std::shared_ptr<IoTDevice>> DeviceManager::getAllDevices() const{
        std::lock_guard<std::mutex> lock(devicesMutex);
        std::vector<std::shared_ptr<IoTDevice>> result;

        for (const auto& pair : devices){
            result.push_back(pair.second);
        }
        return result;
    }

    std::vector<std::string> DeviceManager::getDeviceIds() const {
        std::lock_guard<std::mutex> lock(devicesMutex);
        return deviceIds;
    }
    
    bool DeviceManager::deviceExists(const std::string& deviceId) const {
        std::lock_guard<std::mutex> lock(devicesMutex);
        return devices.find(deviceId) != devices.end();
    }
    
    size_t DeviceManager::getDeviceCount() const {
        std::lock_guard<std::mutex> lock(devicesMutex);
        return devices.size();
    }
    
    std::string DeviceManager::generateDeviceId(const std::string& prefix) {
        std::lock_guard<std::mutex> lock(devicesMutex);
        return prefix + "_" + std::to_string(nextId++);
    }

    bool DeviceManager::sendMessageToDevice(const Message& message){
        std::string destID = message.getDestinationDeviceId();
        auto device = getDevice(destID);

        if(!device){
            std::cerr << "Error: Destionation Deive: " << destID << "not found" << std::endl;
            return false;
        }

        try{
            device->receiveData(message);
            return true;
        } catch (const std::exception& e){
            std::cerr << "Error Sending Message to " << destID << ": " << e.what() << std::endl;
            return false;
        }
    }
    void DeviceManager::broadcastMessage(const Message& message) {
        std::lock_guard<std::mutex> lock(devicesMutex);
        
        for (const auto& pair : devices) {
            try {
                // Don't send message back to source device
                if (pair.first != message.getSourceDeviceId()) {
                    pair.second->receiveData(message);
                }
            } catch (const std::exception& e) {
                std::cerr << "Error broadcasting to " << pair.first << ": " << e.what() << std::endl;
            }
        }
    }
    
    void DeviceManager::listDevices() const {
        std::lock_guard<std::mutex> lock(devicesMutex);
        std::cout << "\n=== Registered Devices (" << devices.size() << ") ===" << std::endl;
        
        for (const auto& pair : devices) {
            std::cout << pair.second->getStatus() << std::endl;
        }
        std::cout << "=========================" << std::endl;
    }
    } // namespace iot
