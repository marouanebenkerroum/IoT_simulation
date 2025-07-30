#ifndef IOT_SIMULATION_ACTUATOR_H
#define IOT_SIMULATION_ACTUATOR_H

#include "../core/IoTDevice.h"

namespace iot {
    
  
    class Actuator : public IoTDevice {
    protected:
        bool state;
        
    public:
        
        Actuator(const std::string& id, const std::string& name);
        
      
        virtual ~Actuator() = default;
        
      
        void sendData() override;
  
        void receiveData(const Message& message) override;
        
     
        virtual void setState(bool newState);
        
       
        bool getState() const { return state; }

        void toggle();
    };
    
} 

#endif 