#ifndef IOT_SIMULATION_CONCRETE_ACTUATORS_H
#define IOT_SIMULATION_CONCRETE_ACTUATORS_H

#include "Actuator.h"

namespace iot {
    
    /**
     * @brief LED Actuator implementation
     */
    class LED : public Actuator {
    private:
        int brightness;  // 0-255
        std::string color;
        
    public:
        LED(const std::string& id, const std::string& name);
        void setState(bool newState) override;
        void setBrightness(int level);
        void setColor(const std::string& newColor);
        int getBrightness() const { return brightness; }
        const std::string& getColor() const { return color; }
    };
    
    /**
     * @brief Motor Actuator implementation
     */
    class Motor : public Actuator {
    private:
        int speed;  // -100 to 100 (negative for reverse)
        int maxSpeed;
        
    public:
        Motor(const std::string& id, const std::string& name, int maxSpd = 100);
        void setState(bool newState) override;
        void setSpeed(int newSpeed);
        void stop();
        int getSpeed() const { return speed; }
        int getMaxSpeed() const { return maxSpeed; }
    };
    
    /**
     * @brief Relay Actuator implementation
     */
    class Relay : public Actuator {
    private:
        double current;
        double maxCurrent;
        bool overloadProtection;
        
    public:
        Relay(const std::string& id, const std::string& name, double maxCurr = 10.0);
        void setState(bool newState) override;
        bool isOverloaded() const;
        double getCurrent() const { return current; }
        double getMaxCurrent() const { return maxCurrent; }
    };
    
} // namespace iot

#endif // IOT_SIMULATION_CONCRETE_ACTUATORS_H