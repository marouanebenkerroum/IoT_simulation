#include "../../include/core/IoTDevice.h"
#include "../../include/core/Message.h"
#include <sstream>
#include <iomanip>
#include <iostream>

namespace iot{
    IoTDevice::IoTDevice(const std::string& id, const std::string& type, const std::string& name)
        : deviceId(id) 
        , deviceType(type)
        , deviceName(name)
        , isActive(true)
        , lastUpdate(std::chrono::steady_clock::now()){}


    std::string IoTDevice::getStatus() const{
        std::ostringstream oss;
        oss << "Device ID: " << deviceId
            << ", Type: " << deviceType
            << ", Name: " << deviceName
            <<", Active: " << (isActive ? "Yes" : "No");
        
        return oss.str();
    }
    void IoTDevice::update(){
        lastUpdate = std::chrono::steady_clock::now();
    }
    }
