#include "SensorVoter.hpp"
#include <math.h>

namespace Joby {

SensorVoter::SensorVoter() : m_threshold(2.0f), m_sensorsAlive(3) {
    m_state[0] = true;
    m_state[1] = true;
    m_state[2] = true;
}

VoterResult SensorVoter::vote(const std::array<float, 3>& raw) {
    bool d12;
    bool d23;
    bool d13;
    VoterResult result;

    switch (m_sensorsAlive){
        case 3:
        d12 = fabs(raw[0] - raw[1]) <= m_threshold;
        d23 = fabs(raw[1] - raw[2]) <= m_threshold;
        d13 = fabs(raw[0] - raw[2]) <= m_threshold;
        if (d12 && d23 && d13) {
            m_state[0] = true;
            m_state[1] = true;
            m_state[2] = true;
        }
        else if (d12 && !d23 && !d13){
            m_state[2] = false;
        }
        else if (!d12 && d23 && !d13){
            m_state[0] = false;
        }
            else if (!d12 && !d23 && d13){
            m_state[1] = false;
        }
        else {
            m_state[0] = false;
            m_state[1] = false;
            m_state[2] = false;
        }
        break;
        case 2:
        if (!m_state[0]) {
            if (fabs(raw[1] - raw[2]) > m_threshold) {
                m_state[1] = false;
                m_state[2] = false;
            }
        }
        else if (!m_state[1]) {
            if (fabs(raw[0] - raw[2]) > m_threshold) {
                m_state[0] = false;
                m_state[2] = false;
            }
        }
        else if (!m_state[2]) {
            if (fabs(raw[0] - raw[1]) > m_threshold) {
                m_state[0] = false;
                m_state[1] = false;
            }
        }
        break;
    }
    
    // Reset bits and sensor alive count
    result.activeSensorMask = 0;
    m_sensorsAlive = 0; 

    float sum = 0;
    for (int i = 0; i < 3; i++) {
        if (m_state[i]) {
            sum += raw[i];
            m_sensorsAlive++;
            result.activeSensorMask |= (1 << i); // SET the bit for the healthy sensor
        }
    }

    if (m_sensorsAlive == 3){
        result.finalTemp = sum / m_sensorsAlive;
        result.confidence = ConfidenceLevel::AllAgree;
    } 
    else if (m_sensorsAlive == 2) {
        result.finalTemp = sum / m_sensorsAlive;
        result.confidence = ConfidenceLevel::MajorityAgree;
    }
    else {
        result.finalTemp = 0;
        result.confidence = ConfidenceLevel::Failure;
    }
    
    return result;
}

} // namespace Joby