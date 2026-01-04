# Pomodoro_Buddies
Pomodoro buddies are synced pomodoro devices allowing parties of up to 3 to use the device at in synced sessions.
**DESIGN OVERVIEW**

This system is designed as a **distributed real-time coordination system** for synchronized Pomodoro sessions across multiple ESP32 devices. The design follows a **single-authority replication model (master/slave)**, where one node (the *coordinator*) maintains the authoritative timer state, and all other nodes (*participants*) act as deterministic replicas of that state.

**Key Design Principles**

  * **Single source of truth** for timing and phase transitions
  * **Relative-time synchronization**, not absolute wall-clock time
  * **Scheduled state transitions** to eliminate network jitter effects
  * **Menu-driven configuration** with persistent local storage
  * **Deterministic finite-state machine** for Pomodoro scheduling

-----

**SYSTEM ARCHITECTURE AND ROLES**

Each ESP32 runs identical firmware and dynamically assumes one of two roles:

**Coordinator**

  * Owns the authoritative Pomodoro state machine
  * Controls phase transitions and scheduling
  * Broadcasts synchronization messages

**Participant**

  * Discovers available sessions
  * Joins a coordinator
  * Renders state and timing based on coordinator messages
  * Does not maintain independent timing logic

Role selection is performed via an on-device OLED menu and stored locally.

-----

**TIME MODEL**

The system achieves synchronization without external time sources. It does **not** use:

  * NTP
  * RTC
  * Internet time
  * Shared Wi-Fi infrastructure

Instead, it relies on:

  * **Monotonic microsecond timers** (`esp_timer_get_time`)
  * **Coordinator-relative timestamps**
  * **Scheduled future execution (ARM semantics)**

All devices map their local time to the coordinator’s timebase using received timestamps, ensuring consistent execution without clock synchronization.

-----

**SYNCHRONIZATION STRATEGY**

The system uses two primary message types for synchronization:

**ARM (Scheduled Transition)**

ARM messages schedule **future phase boundaries** using a shared timestamp. This strategy:

  * Eliminates latency-based desynchronization
  * Ensures all devices transition at the same instant
  * Allows retransmission during a guard interval

**STATE (Authoritative Replication)**

STATE messages are broadcast periodically and contain the current phase, phase start time, phase duration, and a sequence number. Participants resynchronize on each STATE update, enabling:

  * Late join recovery
  * Packet loss tolerance
  * Drift correction

-----

**POMODORO SCHEDULER DESIGN**

The Pomodoro logic is implemented as a **finite state machine** with cycle tracking. The scheduler runs **only on the coordinator**. Participants receive serialized state updates and never compute transitions locally.

**States**

  * IDLE
  * WORK
  * SHORT\_BREAK
  * LONG\_BREAK

**Parameters**

  * Work duration
  * Short break duration
  * Long break duration
  * Cycle length (number of work sessions before long break)

-----

**USER INTERFACE DESIGN**

**Display**

  * SSD1306 OLED (128×64)
  * I2C interface

**Input**

A simple two-button interface is used:

  * Navigate / increment
  * Select / confirm / ready

**Menu Capabilities**

  * Role selection (AUTO / COORDINATOR / PARTICIPANT)
  * Session name selection
  * Timer preset selection
  * Session discovery and join
  * Ready/start control


-----

**WIRELESS COMMUNICATION: ESP-NOW**

**Reasons for Selection:**

ESP-NOW is used for all inter-device communication due to its advantages:

  * No access point required
  * No SSID or IP configuration
  * Low latency
  * Broadcast and unicast support
  * Deterministic behavior in constrained environments

A fixed RF channel is used to ensure reliable communication in different Wi-Fi environments.

-----

**TECHNOLOGY STACK**

**Hardware**

  * ESP32 (WROOM-32)
  * SSD1306 OLED (I2C)
  * Momentary push buttons

**Firmware Framework**

  * Arduino framework for ESP32

**Core Libraries**

| **Library**        | **Purpose**                             |
| :----------------: | :-------------------------------------: |
| `WiFi.h`           | Enable Wi-Fi radio (ESP-NOW dependency) |
| `esp_now.h`        | Peer-to-peer wireless messaging         |
| `esp_timer.h`      | Microsecond-resolution monotonic timing |
| `Preferences.h`    | Non-volatile storage (NVS)              |
| `Adafruit_GFX`     | Display abstraction                     |
| `Adafruit_SSD1306` | OLED driver                             |

**Persistent Configuration**

The following parameters are stored in non-volatile memory, allowing devices to reboot without reconfiguration:

  * Device nickname
  * Default role mode
  * Last-used schedule preset

-----

**RELIABILITY CONSIDERATIONS**

  * Sequence numbers prevent stale message application
  * Periodic STATE messages provide automatic resynchronization
  * Scheduled transitions tolerate packet latency and loss
  * Late-joining devices recover immediately on receipt of STATE
  * Fixed channel configuration avoids environmental instability
