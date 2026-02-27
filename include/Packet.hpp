#include <cstdint>

#pragma pack(push, 1) // No padding!
struct Packet {
    uint8_t sof; // single byte for Start Of Frame
    uint8_t id; // single byte to identify Topic (Altitude, Temp, etc.)
    uint8_t payload[4]; // array to hold data
    uint8_t checksum; // single byte to verify the data has not been corrupted
};
#pragma pack(pop) // Return to normal rules