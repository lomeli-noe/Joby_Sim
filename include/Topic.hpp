#pragma once
#include <cstdint>

namespace Joby {

template <typename T>
class Topic {
using ListenerCallback = void (*)(const T&);
private:
    T m_data;
    uint32_t m_version; // Every time we publish, we increment this
    bool m_hasBeenUpdated;
    uint32_t m_lastUpdateTick;
    ListenerCallback m_listener = nullptr;

public:
    // 1. Constructor: Should take a "defaultValue" and initialize m_version to 0
    Topic(const T& defaultValue) : m_data(defaultValue), m_version(0), m_hasBeenUpdated(false) {}
    
    void registerListener(ListenerCallback cb) {m_listener = cb;}

    // 2. Publish: Should take a "newValue", store it, and increment m_version
    void Publish(const T& newValue, uint32_t currentTick) {
        m_data = newValue;
        m_lastUpdateTick = currentTick;
        m_version++;
        if (m_listener != nullptr) {
            m_listener(m_data);
        }

        if (!m_hasBeenUpdated) {
            Metadata();
        }
    }
    // 3. Subscribe: Should return the current m_data
    T Subscribe() const {
        return m_data;
    }

    T getLastTick() const {
        return m_lastUpdateTick;
    }
    // 4. Metadata: A function to check if the data has EVER been updated
    void Metadata() {
        m_hasBeenUpdated = true;
    }

    uint32_t getVersion() const {
        return m_version;
    }

    bool hasNewData(uint32_t lastKnownVersion) const {
        return m_version > lastKnownVersion;
    }

    bool isStale(uint32_t currentTick, uint32_t timeout) const {
        return (currentTick - m_lastUpdateTick) > timeout;
    }

};

} 