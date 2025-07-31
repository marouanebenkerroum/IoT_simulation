#ifndef IOT_SIMULATION_DEVICE_MANAGER_H
#define IOT_SIMULATION_DEVICE_MANAGER_H

#include "IoTDevice.h"
#include <map>
#include <vector>
#include <memory>
#include <string>
#include <mutex>

namespace iot{

    class Message;

    class DeviceManager{
        private:
            std::map<std::string, std::shared_ptr<IoTDevice>> devices;
            std::vector<std::string> deviceIds;
            mutable std::mutex devicesMutex;
            int nextId;
        public:  
            DeviceManager();

            ~DeviceManager() = default;

            bool registerDevice(std::shared_ptr<IoTDevice> device);


            bool unregisterDevice(const std::string& deviceId);

            std::shared_ptr<IoTDevice> getDevice(const std::string& deviceId) const;

            std::vector<std::shared_ptr<IoTDevice>> getAllDevices() const;

            std::vector<std::string> getDeviceIds() const;

            bool deviceExists(const std::string& deviceId) const;

            size_t getDeviceCount() const;

            std::string generateDeviceId(const std::string& prefix = "DEVICE");

            bool sendMessageToDevice(const Message& message);
            
            void broadcastMessage(const Message& message);

            void listDevices() const;
    };

   
}
#endif