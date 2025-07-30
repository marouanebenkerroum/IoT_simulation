#ifndef IOT_SIMULATION_IOTDEVICE_H
#define IOT_SIMULATION_IOTDEVICE_H

#include <string>
#include <memory>
#include <chrono>


namespace iot {
    class Message;

    class IoTDevice{
        protected:
          std::string deviceId;
          std::string deviceType;
          std::string deviceName;
          bool isActive;
          std::chrono::steady_clock::time_point lastUpdate;

        public: 
            IoTDevice(const std::string& id, const std::string& type, const std::string& name);

            virtual ~IoTDevice() = default;

            virtual void sendData() = 0;
            virtual void receiveData(const Message& message) = 0;
            virtual std::string getStatus() const;

            virtual void update();

            const std::string& getDeviceId() const { return deviceId; }

            const std::string& getDeviceType() const { return deviceType; }

            const std::string& getDeviceName() const { return deviceName; }

            bool isActiveDevice() const { return isActive; }

            void setActive(bool active) { isActive = active; }
    };
}

#endif