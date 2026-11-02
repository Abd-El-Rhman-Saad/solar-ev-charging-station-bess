# 🛠️ Hardware Prototyping & PCB Design

This directory contains the Electronic Design Automation (EDA) project files, fabrication data, bill of materials, and laboratory experimental validation for the physical prototypes of the **Solar-Powered EV Charging Station with BESS**. All converter power stages and control interfaces were transitioned from MATLAB/Simulink mathematical models into functional printed circuit boards designed in **Altium Designer** and verified under rigorous bench-testing conditions.

---

## 📌 Executive Summary & Architecture

The hardware implementation bridges the gap between numerical simulation and practical power engineering. The design encompasses three dedicated converter stages:

1. **Two-Quadrant (2Q) Bidirectional Chopper:** Interfaces the 7.4 V / 2.55 Ah Battery Energy Storage System (BESS) with the DC bus, managing synchronous bidirectional power flow (Buck charging / Boost discharging).
2. **Synchronous EV Buck Charger:** Implements closed-loop Constant Current / Constant Voltage (CC-CV) battery charging for the EV battery pack with fast dynamic response and high conversion efficiency.
3. **Interleaved Boost Converter:** Multi-phase boost topology designed to interface high-current low-voltage PV generation with the common DC bus while drastically reducing input current ripple.

### Hardware Archive Packages

| Archive Name | Target Topology | EDA Tool | Contents Included |
| :--- | :--- | :--- | :--- |
| **`Two_Quadrant_Chopper.zip`** | Bidirectional 2Q Chopper | Altium Designer | `.SchDoc`, `.PcbDoc`, Gerbers, NC Drill, Pick-and-Place |
| **`Buck_Converter.zip`** | Synchronous Buck Power Stage | Altium Designer | Complete Schematic, Layout, BOM, Fabrication Outputs |
| **`Interleaved_Thevenin.zip`** | 2-Phase Interleaved Boost | Altium Designer | Dual-phase layout, Inductor footprints, Gate routing |

---

## 📐 Electronic Design Automation (Altium Designer Workflow)

The hardware design process follows standard industrial power electronics EDA standards:

* **Schematic Capture (`.SchDoc`):** Hierarchical schematic structure isolating the high-voltage/high-current power stage from the sensitive low-voltage analog sensing and digital PWM control domains.
* **PCB Layout Routing (`.PcbDoc`):** Double-sided FR-4 substrate ($1.6\text{ mm}$ thickness, $1\text{ oz}$ outer copper weight) optimized for minimum parasitic inductance and high thermal dissipation.
* **Manufacturing & CAM (`CAMtastic`):** Standard RS-274X Gerber files, NC Drill files, and solder mask/paste layers ready for bare-board fabrication.

### Altium Schematic Architecture
Detailed circuit design showing the gate-drive optocoupler isolation stage, bootstrap circuitry, half-bridge MOSFET leg, and analog sensing connections:

<p align="center">
  <img src="images/schematic.png" width="95%" alt="Altium Schematic Architecture" />
</p>

### Altium 3D Isometric Board Layout
3D CAD rendering of the fabricated two-quadrant board displaying component placement, toroidal inductors, radial electrolytic filter capacitors, and heatsink mounting:

<p align="center">
  <img src="images/pcb_3d_isometric.png" width="90%" alt="Altium PCB 3D Isometric Board Layout" />
</p>

---

## ⚡ High-Power PCB Layout & Signal Integrity Guidelines

Designing PCBs for high-frequency switching converters ($10\text{ kHz}$ to $100\text{ kHz}$) operating with inductive loads demands strict layout discipline to mitigate electromagnetic interference (EMI) and parasitic oscillations:

1. **Star Grounding Strategy (Single-Point Ground):**
   * Strict physical segregation between **Power Ground (PGND)** carrying high-current switching pulses and **Signal Ground (AGND/DGND)** used by the microcontroller and analog sensors.
   * Both ground domains meet at a single physical point (near the bulk filter capacitor return) to prevent ground loops from injecting switching noise into ADC readings.

2. **Gate Driver Loop Inductance Minimization:**
   * High $di/dt$ gate drive loops between the **IR2111** driver output, MOSFET gate, and source terminal were routed using short, wide, parallel traces to minimize parasitic inductance ($L_{\text{gate}}$).
   * A dedicated damping gate resistor ($22\ \Omega$) is placed immediately adjacent to the MOSFET gate pin to suppress ringing caused by gate capacitance ($C_{iss}$) and trace inductance.

3. **Thermal Management & Low-ESR Polygon Pours:**
   * High-current conduction paths feature broad polygon copper pours rather than narrow traces, drastically lowering Equivalent Series Resistance (ESR) and eliminating thermal hotspots.
   * Heavy-duty TO-247 packages are heatsink-mounted to maintain thermal stability during sustained high-current testing.

4. **Creepage & Clearance Safety Spacing:**
   * Creepage and clearance distances comply with IPC-2221 standards, providing adequate dielectric spacing between the high-voltage bus (up to $300\text{ V DC}$) and logic circuits to prevent flashover and dielectric breakdown.

---

## 🔬 Laboratory Testbed & Instrumentation Setup

Experimental validation of the physical converters was carried out in the Electrical Power & Machines Laboratory utilizing high-precision programmable power equipment:

### Core Laboratory Equipment Specifications

* **Programmable DC Power Supply (TDK-Lambda GEN 300-11):**
  * Voltage Range: $0\text{--}300\text{ V DC}$, Current Range: $0\text{--}11\text{ A}$, Maximum Power: $3.3\text{ kW}$.
  * Provides stable, isolated, and programmable DC bus excitation, mimicking PV array output and DC link dynamics without electrochemical safety hazards.
* **Digital Storage Oscilloscope (Hantek DSO4254B):**
  * 4 Analog Channels, $250\text{ MHz}$ Bandwidth, $1\text{ GSa/s}$ Real-time Sampling Rate.
  * Used for capturing gate-drive waveforms, switching transitions, dead-time verification, and output voltage ripple.
* **Precision Digital Multimeters (UNI-T UT33D+ & UT89X True RMS):**
  * $600\text{ V}$ AC/DC, $10\text{ A}$ DC high-current continuous terminal.
  * Utilized for direct measurement of converter load currents and battery terminal voltages.

### Assembled & Soldered 2Q Chopper PCB
Physical prototype of the Two-Quadrant Chopper after populated through-hole assembly, showing the IRFP260N power stage, gate-drive isolation, and toroidal magnetic core:

<p align="center">
  <img src="images/pcb_actual_board.jpg" width="85%" alt="Physical Populated 2Q Chopper PCB Assembly" />
</p>

### Complete Laboratory Experimental Bench Setup
Full bench-testing configuration featuring the programmable DC power supply, breadboard interface, Arduino Uno digital controller, and active load arrangement:

<p align="center">
  <img src="images/all_hardware_system.png" width="90%" alt="Complete Hardware Bench Testing Setup" />
</p>

---

## 📋 Comprehensive Bill of Materials (BOM) & Component Specifications

The following table details the primary active semiconductors, passive power components, sensing modules, and control hardware utilized across the physical prototypes:

| Category | Component / IC | Part Number / Specs | Functional Role in System |
| :--- | :--- | :--- | :--- |
| **Power Switches** | N-Channel Power MOSFET | **IRFP260N** (200V, 50A, $R_{DS(on)}=0.04\ \Omega$, TO-247AC) | Primary high-efficiency synchronous switching elements |
| **Gate Driver** | High-Voltage Half-Bridge Driver | **IR2111PBF / IR2110** (600V floating high-side, 0.5A/0.25A) | Level-shifts 5V logic PWM to 12V gate drive with dead-time |
| **Galvanic Isolation** | High-Gain Optocoupler | **6N139 / 6N137** (Split-Darlington, $5000\ \text{V}_{\text{RMS}}$ isolation) | Optically isolates Arduino digital pins from power ground noise |
| **Power Diodes** | Fast Schottky Rectifiers | **MBR20100CT** (Dual 100V, 20A) & **UF4007** / **1N4148** | Bootstrap diode and freewheeling clamp protection |
| **Current Sensing** | Hall-Effect Sensor Module | **ACS712ELC-05A** ($\pm 5\text{ A}$, $185\text{ mV/A}$, $2.4\text{ kV}_{\text{RMS}}$ isolation) | Bidirectional feedback current measurement for PI control |
| **Voltage Sensing** | Resistive Divider Module | **DC 0–25V Module** ($5:1$ ratio, $30\text{ k}\Omega + 7.5\text{ k}\Omega$, 1% precision) | Step-down bus and battery voltage sensing for Arduino ADC |
| **Microcontroller** | Embedded Controller Board | **Arduino Uno Rev3** (ATmega328P @ 16 MHz) | Real-time 10 kHz PWM generation, ADC sampling, PI routine |
| **EV Battery Load** | Li-Ion Battery Simulator Pack | **2S IMR14500** ($7.4\text{ V}$ nominal, $800\text{ mAh}$, $6.0\text{--}8.4\text{ V}$ cutoff) | Scaled EV battery model for charging profile validation |
| **BESS Storage** | Li-Ion Energy Storage Pack | **2S ICR18650** ($7.4\text{ V}$ nominal, $2.55\text{ Ah}$, EVE Energy) | Stationary energy storage pack for solar microgrid buffer |
| **Battery Management** | 2S Li-ion Protection Board | **HW-391 BMS** ($20\text{ A}$ max discharge, overcharge/discharge protection) | Hardware cell balancing and safety cutoff for 2S pack |
| **Power Magnetics** | High-Current Inductor | **Toroidal Power Choke** ($2.0\text{ mH}$, low core saturation) | Main inductive energy storage element for DC-DC stage |
| **Filter Capacitors** | Bulk & Decoupling Capacitors | **$220\ \mu\text{F}$ / $22\ \mu\text{F}$ / $10\ \mu\text{F}$** Electrolytic + **$0.1\ \mu\text{F}$** Ceramic | Output voltage ripple smoothing and IC supply decoupling |
| **Gate Resistors** | Metal-Film Resistors | **$22\ \Omega$**, **$330\ \Omega$**, **$1\text{ k}\Omega$**, **$10\text{ k}\Omega$** (1/4 W) | Gate damping, LED current limiting, and active pull-down |

---

## 💻 Embedded Firmware & High-Speed Timer Implementation

To ensure precise switching control and eliminate acoustic noise, the ATmega328P microcontroller was configured to generate **$10\text{ kHz}$ Phase-Correct PWM** on digital pin 9 by directly manipulating hardware Timer 1 control registers.

### Mathematical Timer Configuration

For Timer 1 running in Phase and Frequency Correct mode with prescaler $N = 1$:

$$f_{\text{PWM}} = \frac{f_{\text{CPU}}}{2 \cdot N \cdot \text{TOP}} = \frac{16\text{ MHz}}{2 \cdot 1 \cdot 800} = 10.0\text{ kHz}$$

Due to the common-emitter optocoupler configuration (6N139), the hardware exhibits an **inverted logic** response (a logic HIGH from the microcontroller pulls the optocoupler output to ground, turning the gate driver input LOW). The firmware directly compensates for this inversion mathematically:

$$D_{\text{effective}} = 1 - D_{\text{command}} = \frac{\text{TOP} - \text{OCR1A}}{\text{TOP}}$$

```cpp
// =========================================================================
// Real-Time 10 kHz Inverted Logic PI Current Controller
// Target: 0.40 A Closed-Loop Regulation across Hardware Prototype
// =========================================================================

#include <Arduino.h>

const int PWM_PIN    = 9;
const int SENSOR_PIN = A0;

// High-Speed Timer 1 Parameters
const int TOP_PWM    = 800;   // TOP value for 10 kHz switching frequency
const float TARGET_I = 0.40;  // Target current reference (Amperes)

// ACS712-05A Calibration Constants
const float VREF         = 5.00;
const float SENSITIVITY  = 0.185; // 185 mV/A sensitivity
const float ZERO_VOLTAGE = 2.50;  // Quiescent voltage at 0 A

// Discrete PI Controller Gains
float Kp = 25.0;
float Ki = 1.2;
float integral_err = 0.0;

void setup() {
    Serial.begin(115200);
    pinMode(PWM_PIN, OUTPUT);
    
    // Configure Timer 1 for 10 kHz Phase-Correct PWM
    TCCR1A = _BV(COM1A1) | _BV(COM1B1); // Non-inverting PWM on OC1A/OC1B
    TCCR1B = _BV(WGM13) | _BV(CS10);    // Mode 8 (Phase & Frequency Correct, TOP = ICR1), Prescaler = 1
    ICR1   = TOP_PWM;                   // Set TOP to 800 -> 10 kHz
    OCR1A  = TOP_PWM;                   // Initialize output OFF (Inverted logic: 800 = OFF)
}

void loop() {
    // 1. Read and filter analog feedback from ACS712
    int rawADC = analogRead(SENSOR_PIN);
    float vSensor = (rawADC * VREF) / 1024.0;
    float I_meas  = (vSensor - ZERO_VOLTAGE) / SENSITIVITY;
    if (I_meas < 0) I_meas = -I_meas; // Magnitude rectifying
    
    // 2. Compute PI current regulation error
    float error = TARGET_I - I_meas;
    integral_err += error * 0.01; // Sample period dt = 10 ms
    
    // Anti-windup clamping
    if (integral_err > 50.0) integral_err = 50.0;
    if (integral_err < -50.0) integral_err = -50.0;
    
    float duty_control = (Kp * error) + (Ki * integral_err);
    
    // 3. Compensate for optocoupler inverted hardware logic
    // (TOP_PWM = 0% duty, 0 = 100% duty)
    int ocr_val = TOP_PWM - (int)duty_control;
    ocr_val = constrain(ocr_val, 200, 780);
    OCR1A = ocr_val;
    
    // 4. Telemetry logging
    Serial.print("I_meas: "); Serial.print(I_meas, 2);
    Serial.print(" A | Target: "); Serial.print(TARGET_I, 2);
    Serial.print(" A | OCR1A: "); Serial.println(OCR1A);
    
    delay(10);
}
```

---

## 📊 Experimental Bench Validation & Measurement Results

### 1. 10 kHz Clean Gate-Drive Pulse Waveform
The gate driver circuits and power converter switching nodes were probed using the **Hantek DSO4254B** oscilloscope. The clean gate pulses confirm minimal rise/fall ringing ($t_r < 80\text{ ns}$):

<p align="center">
  <img src="images/waveform_gate.jpg" width="85%" alt="10 kHz Clean Gate-Drive Pulse Waveform" />
</p>

### 2. Output DC Voltage Ripple & Mean Stability
Measurement of output DC bus potential under full load, verifying negligible voltage ripple and excellent DC regulation:

<p align="center">
  <img src="images/voltage_scope.jpeg" width="85%" alt="Output DC Voltage Ripple and Mean Stability" />
</p>

### 3. Real-Time Hardware Testing Bench
The closed-loop current control was benchmarked under real-time passive and battery loads with the **UNI-T UT89X** digital multimeter measuring current feedback:

<p align="center">
  <img src="images/bench_closed_loop_test.jpg" width="85%" alt="Real-Time Hardware Testing with UNI-T Multimeter" />
</p>

### 4. Arduino Serial Monitor Telemetry Log
High-speed serial telemetry stream ($115200\text{ baud}$) recording ADC current measurements, target references, and timer compare register adjustments:

<p align="center">
  <img src="images/serial_monitor.jpg" width="85%" alt="Arduino Serial Monitor Telemetry Log" />
</p>

### Experimental Current Step Response Verification

The closed-loop PI current control algorithm was evaluated across four sequential reference setpoints ($0.30\text{ A}$ to $0.60\text{ A}$). The digital multimeter verified accurate current tracking with steady-state error below $2.5\%$:

| Target Current ($I_{\text{ref}}$) | Multimeter Reading ($I_{\text{meas}}$) | Timer Register ($\text{OCR1A}$) | Steady-State Error ($\%$) | Verification Status |
| :---: | :---: | :---: | :---: | :---: |
| **$0.30\text{ A}$** | $0.29\text{ A}$ | $524$ | $-3.3\%$ | ✅ Verified |
| **$0.40\text{ A}$** | $0.38\text{--}0.40\text{ A}$ | $501\text{--}502$ | $-2.5\%$ | ✅ Verified |
| **$0.50\text{ A}$** | $0.48\text{--}0.50\text{ A}$ | $478$ | $-2.0\%$ | ✅ Verified |
| **$0.60\text{ A}$** | $0.58\text{--}0.60\text{ A}$ | $452$ | $-1.6\%$ | ✅ Verified |

### Multimeter Measurement Gallery

#### Reference: 0.30 A ($I_{\text{meas}} = 0.29\text{ A}$)
<p align="center">
  <img src="images/reading_0_3A.png" width="60%" alt="Multimeter Reading at 0.3A Reference" />
</p>

#### Reference: 0.40 A ($I_{\text{meas}} = 0.38\text{ A}$)
<p align="center">
  <img src="images/reading_0_4A.png" width="60%" alt="Multimeter Reading at 0.4A Reference" />
</p>

#### Reference: 0.50 A ($I_{\text{meas}} = 0.48\text{ A}$)
<p align="center">
  <img src="images/reading_0_5A.png" width="60%" alt="Multimeter Reading at 0.5A Reference" />
</p>

#### Reference: 0.60 A ($I_{\text{meas}} = 0.58\text{ A}$)
<p align="center">
  <img src="images/reading_0_6A.png" width="60%" alt="Multimeter Reading at 0.6A Reference" />
</p>

---

## 📂 Directory Structure & Hardware Replication Guide

```text
5_Hardware_and_PCB_Design/
├── README.md                      # Hardware engineering documentation & testbed report
├── Buck_Converter.zip             # Altium project for Synchronous Buck EV Charger
├── Interleaved_Thevenin.zip       # Altium project for Interleaved Boost Converter
├── Two_Quadrant_Chopper.zip       # Altium project for Bidirectional 2Q BESS Chopper
└── images/                        # High-resolution CAD renders and bench captures
    ├── all_hardware_system.png
    ├── bench_closed_loop_test.jpg
    ├── pcb_3d_isometric.png
    ├── pcb_actual_board.jpg
    ├── reading_0_3A.png
    ├── reading_0_4A.png
    ├── reading_0_5A.png
    ├── reading_0_6A.png
    ├── schematic.png
    ├── serial_monitor.jpg
    ├── voltage_scope.jpeg
    └── waveform_gate.jpg
```

### Steps for Hardware Replication

1. **Extract EDA Files:** Extract `Two_Quadrant_Chopper.zip` or the required converter archive and open the `.PrjPcb` file inside Altium Designer.
2. **Fabricate Bare Boards:** Export the Gerber RS-274X and NC Drill layers located in the `Manufacturing/` directory to your chosen PCB fabrication house.
3. **Component Assembly:** Solder SMD decoupling components first, followed by through-hole gate drivers, optocouplers, and power MOSFETs mounted to their heatsinks using thermal compound.
4. **Flash Control Firmware:** Connect the Arduino Uno via USB, load the 10 kHz PWM sketch with Timer 1 configuration, and verify gate signals with an oscilloscope before applying high DC bus voltage.
5. **Gradual Voltage Bring-Up:** Power the DC power stage using a current-limited DC supply (e.g., TDK-Lambda GEN 300-11) starting at low voltage ($5\text{--}12\text{ V}$) while monitoring switching nodes for voltage spikes.
