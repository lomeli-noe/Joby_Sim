# Aerospace Flight Framework & Distributed Middleware (Modern C++)

A high-reliability, deterministic software framework designed for eVTOL flight control systems. This project implements a layered architecture including a type-safe Pub/Sub middleware, a triple-redundant voting system, and a robust telemetry serialization protocol.

## ✈️ Framework Overview

This framework serves as a proof-of-concept for distributed aircraft computing. It addresses the critical challenges of data integrity, inter-module coupling, and deterministic execution in safety-critical environments.

### Core Engineering Pillars

- **Deterministic Memory**: Zero dynamic memory allocation (no heap/new/malloc) after initialization to prevent fragmentation and non-deterministic latencies.
- **Decoupled Middleware**: An event-driven Pub/Sub architecture utilizing C++ templates to enable type-safe communication between isolated flight modules.
- **Redundancy Management**: A Triple-Redundant Voter with latching fault logic to ensure system-wide stability during sensor excursions.
- **Defense in Depth**: A multi-layered safety approach where data is validated at the communication layer (Checksums) and the logic layer (Voter).

## 📋 System Requirements (DO-178C Traceability)

Developed with a focus on safety-critical standards, the system satisfies the following decomposed requirements:

| ID | Requirement | Compliance Level |
|---|---|---|
| REQ-01 | The system shall execute a non-blocking flight loop at a deterministic frequency (10Hz). | High |
| REQ-02 | The middleware shall support type-safe data distribution via a centralized static FlightBus. | High |
| REQ-03 | Data integrity shall be verified using a checksum-validated serialization protocol. | Low |
| REQ-04 | The system shall identify and isolate sensor outliers via a Majority-Vote algorithm. | High |
| REQ-05 | Latching Faults: Outliers shall be permanently excluded from the control loop until a system reset. | High |
| REQ-06 | Failsafe: The system shall enter a safe state (0.0 output) if a consensus cannot be reached. | High |

## 🏗️ Architectural Breakdown

### 1. Distributed Middleware (Topic<T> & FlightBus)

- Utilizes C++ Templates and function pointers to create a "Global Data Bus."
- Event-Driven: Modules register callbacks to respond to data updates instantly.
- Temporal Logic: Integrated Watchdog timers monitor m_version and m_lastUpdateTick to detect stale data in distributed nodes.

### 2. Redundancy Logic (SensorVoter)

- The "Last Line of Defense" for sensor data.
- Survival Modes: Seamlessly transitions between ALL_AGREE, MAJORITY_AGREE (isolated outlier), and FAILURE (system shutdown).
- Static Health Mask: Provides bitwise visibility into which specific sensors are untrusted.

### 3. Serialization Layer (Packet)

- Simulates data bus communication (CAN/ARINC 429).
- Bit-Packing: Telemetry is packed into 32-bit words, including 16-bit scaled integers, 2-bit confidence codes, and 3-bit health masks.
- Binary Integrity: Implements #pragma pack(1) to ensure binary compatibility across heterogeneous CPU architectures (OS-Independent API).

## 🧪 Verification & Stress Testing

The system includes a Mission Simulation loop that performs automated Fault Injection.

### The "Stealth Error" Test

I implemented an "Evil Noise" injector designed to flip bits that trick a standard Sum-Checksum.

**The Problem**: Noise flips bits in a way that the mathematical sum remains identical. The communication layer reports a "Pass."

**The Solution**: The SensorVoter identifies that the "validated" data is a logical outlier compared to the 3-sample history window and rejects the data.

**Test Result**:
```
Cycle 10 | Received: 0.00012 | Checksum: Pass | Status: MAJORITY | Mask: 0x3
>>> [VOTER] HERO MOMENT: Stealth corruption detected and isolated!
```

This verifies the "Defense in Depth" strategy: the Voter catches what the Checksum misses.

## 💻 Tech Stack

- **Languages**: Modern C++ (C++11/17)
- **Hardware Target**: ATmega2560 (8-bit AVR) & x86/ARM Simulation
- **Toolchain**: PlatformIO, CMake, g++, avr-gcc
- **Design Patterns**: Interface-based Polymorphism, Dependency Injection, Circular Buffers (O(1)).

## 🚀 Getting Started

- **Desktop Sim**: Run `g++ -std=c++17 -I./include src/*.cpp -o flight_sim && ./flight_sim` to view the fault injection stress test.
- **Embedded Target**: Open in VS Code with PlatformIO, connect an Arduino Mega, and upload to view real-time hardware ADC sampling.
