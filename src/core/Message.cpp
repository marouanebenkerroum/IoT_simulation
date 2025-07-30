#include "../../include/core/Message.h"
#include <sstream>
#include <iomanip>
#include <random>
#include <algorithm>

namespace iot
{
    Message::Message(const std::string& sourceId, 
                const std::string& destId,
                const std::string& data,
                MessageType msgType)
        : sourceDeviceId(sourceId),
        destinationDeviceId(destId),
        payload(data),
        type(msgType),
        timestamp(std::chrono::steady_clock::now()){
        
        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::uniform_int_distribution<> dis(100000,999999);
        messageId = "MSG_" + std::to_string(dis(gen));
     }

     void Message::addHeader(const std::string& key, const std::string& value){
        headers[key] = value;
     }

     std::string Message::getHeader(const std::string& key) const {
        auto it = headers.find(key);
        if(it != headers.end()){
            return it->second;
        }
        return "";
     }

     bool Message::hasHeader(const std::string& key) const{
        return headers.find(key) != headers.end();
     }
     std::string Message::toString() const {
        std::ostringstream oss;
        oss << "Message[ID: " << messageId 
            << ", From: " << sourceDeviceId 
            << ", To: " << destinationDeviceId 
            << ", Type: " << static_cast<int>(type)
            << ", Payload: " << payload << "]";
        return oss.str();
    }
  } // namespace iot
