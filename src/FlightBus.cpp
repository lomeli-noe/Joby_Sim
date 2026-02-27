#include "FlightBus.hpp"

namespace Joby {
    // This is where the memory is actually reserved
    Topic<float> FlightBus::Altitude(0.0f);
    Topic<float> FlightBus::Temperature(20.0f);
    Topic<uint8_t> FlightBus::MotorState(0);
}