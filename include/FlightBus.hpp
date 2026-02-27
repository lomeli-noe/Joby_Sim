#include "Topic.hpp"

namespace Joby {

class FlightBus {
public:
    // We make these 'static' so they exist everywhere in the program
    // without needing to pass a "Bus" object around.
    static Topic<float> Altitude;
    static Topic<float> Temperature;
    static Topic<uint8_t> MotorState;
    static Topic<uint8_t> BatteryPct;
};

} // namespace Joby