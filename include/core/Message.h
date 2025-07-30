#ifndef IOT_SIMULATION_MESSAGE_H
#define IOT_SIMULATION_MESSAGE_H

#include <string>
#include <chrono>
#include <map>

namespace iot{
    class Message{
        public:
            enum class MessageType{
                DATA,
                COMMAND,
                ACKNOWLEDGMENT,
                ERROR
            };
        
        private: 
            std::string messageId;
            std::string sourceDeviceId;
            std::string destinationDeviceId;
            std::string payload;
            MessageType type;
            std::chrono::steady_clock::time_point timestamp;
            std::map<std::string, std::string> headers;

        public: 
            Message(const std::string& sourceId, 
                const std::string& destId,
                const std::string& data,
                MessageType msgType =  MessageType::DATA);

            ~Message() = default;

            const std::string& getMessageId() const { return messageId; }
            const std::string& getSourceDeviceId() const { return sourceDeviceId; }
            const std::string& getDestinationDeviceId() const { return destinationDeviceId; }
            const std::string& getPayload() const { return payload; }
            MessageType getMessageType() const { return type; }      
            std::chrono::steady_clock::time_point getTimestamp() const { return timestamp; }
         
            void setPayload(const std::string& data) { payload = data; }
            
            void addHeader(const std::string& key, const std::string& value);
            std::string getHeader(const std::string& key) const;
            bool hasHeader(const std::string& key) const;
            
            std::string toString() const;
    };
}

#endif 