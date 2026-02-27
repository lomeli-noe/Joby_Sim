#include "FlightBus.hpp"
#include "MessageBuffer.hpp"
#include "Packet.hpp"
#include "SensorVoter.hpp"
#include <iostream>
#include <cstring>
#include <random>
#include <array>

using namespace std;

Joby::MessageBuffer<float, 10> altBuffer;

void onAltUpdate(const float& val) {
    altBuffer.push(val);
}

Packet createPacket(uint8_t id, float value) {
    Packet result;
    result.sof = 0xAA;
    result.id = id;
    std::memcpy(result.payload, &value, sizeof(float));
    result.checksum = result.sof + result.id + result.payload[0] + result.payload[1] + result.payload[2] + result.payload[3];

    return result;
}

bool parsePacket(const Packet& p, float& outValue) {
    uint8_t sum = p.sof + p.id + p.payload[0] + p.payload[1] + p.payload[2] + p.payload[3];
    if (p.sof != 0xAA || sum != p.checksum) {
        return false;
    }
    else {
        std::memcpy(&outValue, p.payload, sizeof(float));
    }
    return true;
}

Packet injectEvilNoise(Packet p) {
    int a = 2;
    int b = 3;

    p.payload[a] += 10; // Increment one byte
    p.payload[b] -= 10; // Decrement another byte

    // The sum of (payload[a] + payload[b]) remains the same!
    return p;
}

float generateRandomFloat(float low, float high) {
    // Use a static random device and engine to maintain state across calls
    static std::random_device rd;
    static std::mt19937 engine(rd()); // Seed the engine with a random device
    std::uniform_real_distribution<float> dist(low, high); // Define the range

    return dist(engine); // Generate and return the random float
}

void updateHistory(float newVal, std::array<float, 3>& history) {
    history[0] = history[1];
    history[1] = history[2];
    history[2] = newVal;
}

string dataLogger(Joby::ConfidenceLevel confidence) {
switch (confidence){
    case Joby::ConfidenceLevel::AllAgree:
    return "All";
    case Joby::ConfidenceLevel::MajorityAgree:
    return "Majority";
    case Joby::ConfidenceLevel::Failure:
    return "Failure";
    default:
    return "Unknown";
}
}

int main() {

    // uint32_t myLastAltVersion = 0;
    // float myCurrentAlt = 0;
    // Joby::FlightBus::Altitude.registerListener(onAltUpdate);

    // for (int i = 0; i < 20; ++i) {
    //     myCurrentAlt += 100;
    //     if (i < 10) {
    //         Joby::FlightBus::Altitude.Publish(myCurrentAlt, i);
    //     }
    //     if (Joby::FlightBus::Altitude.isStale(i, 3)) {
    //         std::cout << "[ALARM] Altitude Data is STALE! Last update at tick: " << Joby::FlightBus::Altitude.getLastTick() << std::endl;
    //     }
    //     else {
    //         std::cout << "System Healthy. Alt: " << myCurrentAlt << std::endl;
    //     }
    // }

    // while (!altBuffer.isEmpty()) {
    //     std::cout << "Processing from Buffer: " << altBuffer.pop() << std::endl;
    // }

    // Packet p = createPacket(1, 500.0f);

    // // print the Hex dump
    // uint8_t* rawBytes = reinterpret_cast<uint8_t*>(&p);
    // std::cout << "Packet Hex: ";
    // for (int i = 0; i < sizeof(Packet); ++i) {
    //     // Show each byte in Hex
    //     std::cout << std::hex << static_cast<int>(rawBytes[i]) << " ";
    // }
    // std::cout << std::endl;

    // -------------------------

    float min_val = 500.0f;
    float max_val = 501.0f;
    int validCount = 0;
    int corruptCount = 0;
    int undetected = 0;
    int cycles = 20;

    Joby::SensorVoter voter;
    std::array<float, 3> flightHistory = {500.0f, 500.0f, 500.0f};
    Joby::VoterResult result;

    std::cout << "Running Stress Test (" << cycles << " cycles)..." << std::endl;
    cout << "Cycle | Received Val | Checksum | Voter Status | Health Mask (Hex)" << endl;
    for (int i = 0; i < cycles; ++i) {
        float original_alt = generateRandomFloat(min_val, max_val);
        Packet p = createPacket(1, original_alt);

        bool wasCorrupted = (i % 5 == 0);
        if (wasCorrupted) p = injectEvilNoise(p);

        float received_alt; 
        bool accepted = parsePacket(p, received_alt);

        if (!accepted) {
            cout << "[BUS] Packet dropped by Checksum." << endl;
        }
        else{
            updateHistory(received_alt, flightHistory);
            result = voter.vote(flightHistory);

            if (wasCorrupted && result.confidence == Joby::ConfidenceLevel::MajorityAgree) {
                cout << ">>> [VOTER] HERO MOMENT: Stealth corruption detected and isolated!" << endl;
            }
        }
        string didPass = accepted ? "Pass" : "Fail";
        cout << i << " | " << received_alt << " | " << (accepted ? "Pass" : "Fail") 
             << " | " << dataLogger(result.confidence) 
             << " | Mask: 0x" << hex << static_cast<int>(result.activeSensorMask) << dec << endl;

        if (!wasCorrupted && accepted) validCount++;    // Happy Path
        if (wasCorrupted && !accepted) corruptCount++;  // Safety working correctly
        if (wasCorrupted && accepted) undetected++;    // THE DANGER: Checksum failed to catch noise!
    }


    std::cout << "[PASS] Correctly identified " << validCount << " valid packets." << std::endl;
    std::cout << "[PASS] Correctly rejected " << corruptCount << " corrupted packets." << std::endl;
    std::cout << "[ALARM] Undetected errors: " << undetected << std::endl;

    return 0;
}