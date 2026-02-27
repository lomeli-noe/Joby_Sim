#pragma once
#include <stdint.h>
#include <array>

namespace Joby {
enum class ConfidenceLevel {
    AllAgree,
    MajorityAgree,
    Failure
};

struct VoterResult {
    ConfidenceLevel confidence;
    float finalTemp;
    uint8_t activeSensorMask; // Bit 0: S1, Bit 1: S2, Bit 2: S3
};

class SensorVoter {
    private:
        const float m_threshold;
        int m_sensorsAlive;
        bool m_state[3];

    public:
        SensorVoter();
        VoterResult vote(const std::array<float, 3>& raw);
};
}