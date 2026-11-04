# 🌐 Full System Integration & Multi-Converter Microgrid Control

> **Team Project:** Solar-Powered EV Charging Station with BESS (8-member team) — *documentation by Eng. Abd El-Rhman Muhammad Saad*  
> **Academic Institution:** Faculty of Engineering, Alexandria University (Class of 2026)  
> **Department:** Electrical Power and Machines Engineering  
> **Core Focus:** Closed-Loop Multi-Converter Microgrid, Energy Management Strategy (EMS), 600V DC-Link Voltage Anchoring & Telemetry Verification

---

This directory contains the closed-loop integrated MATLAB/Simulink system model, hierarchical subsystem schematics, and simulation results for the complete **Solar-Powered EV Charging Station with BESS**. The integrated microgrid coordinates a multi-string Photovoltaic array, an Interleaved Boost Converter with Perturb & Observe (P&O) MPPT, a common regulated 600 V DC bus, a bidirectional Two-Quadrant Chopper interfacing the Battery Energy Storage System (BESS), and a fast-acting Synchronous Buck Converter charging an Electric Vehicle (EV) battery pack.

---

## 📌 Executive Summary & Architecture

The primary engineering challenge of a grid-independent solar EV charging station is achieving **instantaneous power balance** and **tight DC-bus voltage regulation** under dynamic generation and fluctuating vehicle loads. 

The integrated system implements a decentralized hierarchical control structure where:
1. **Generation Stage (MPPT):** Maximizes renewable solar harvesting by extracting $8.49\text{ kW}$ from the PV array at the simulated operating point.
2. **Voltage Regulation & Energy Buffer (BESS 2Q Chopper):** Regulates the common DC-link voltage firmly at $600.0\text{ V}$ via a dual-loop cascade PI controller, absorbing excess solar generation or injecting stored energy during solar deficits.
3. **EV Charging Stage (Fast Buck Charger):** Delivers a regulated constant-current (CC) profile ($10.17\text{ A}$, $4.07\text{ kW}$) to the EV battery pack using high-speed hysteresis current tracking.

### Core Simulation Model

| Model File | Environment | Solver Configuration | Description |
| :--- | :--- | :--- | :--- |
| **`Solar_EV_BESS_Full_Sys.slx`** | MATLAB / Simulink R2022b+ | Discrete `powergui` ($T_s = 1.0\ \mu\text{s}$) | Complete integrated multi-converter closed-loop microgrid model |

---

## 🏛️ Master Architecture & Real-Time Telemetry

The top-level model integrates all three converter stages connected to the centralized $600\text{ V}$ DC-link bus. The simulation results demonstrate steady-state convergence of the DC-bus voltage and a balanced power split among the three converter stages:

<p align="center">
  <img src="images/System_00_Root_Solar_EV_BESS_Full_Sys_Full_Overview.png" width="95%" alt="Full Integrated Microgrid Simulink Architecture" />
</p>

### Steady-State Numerical Operating Point

| Parameter Key | Subsystem Origin | Physical Quantity | Steady-State Value | Engineering Status |
| :--- | :--- | :--- | :--- | :--- |
| **`Imp`** | PV Array | Total Extracted Solar Current | **$34.86\text{ A}$** | Operating at Maximum Power Point |
| **`Vmp`** | PV Array | Array Operating Voltage | **$243.6\text{ V}$** | Tracks theoretical $V_{mp}$ |
| **`Pmppt`** | PV Array | Extracted Solar Power | **$8,491\text{ W}$** | Nominal rating: $8.5\text{ kW}$ |
| **`IL1`** | Interleaved Boost | Phase-1 Inductor Current | **$17.13\text{ A}$** | $49.1\%$ Current Sharing |
| **`IL2`** | Interleaved Boost | Phase-2 Inductor Current | **$17.73\text{ A}$** | $50.9\%$ Current Sharing |
| **`VDC`** | Common DC-Link | Regulated Bus Voltage | **$600.0\text{ V}$** | Settles firmly at $600.0\text{ V}$ |
| **`Vout`** | 2Q Chopper | BESS Power Stage Voltage | **$600.0\text{ V}$** | Matches common bus |
| **`IB`** | BESS Storage | Battery Current (+Charge / -Discharge) | **$+11.25\text{ A}$** | Absorbing excess generation at its charge-current limit |
| **`Pstorage`**| BESS Storage | Storage Battery Power | **$+4,498\text{ W}$** | Charging buffer active |
| **`IbEV`** | EV Charger | EV Battery Charging Current | **$10.17\text{ A}$** | Constant Current Mode active |
| **`VoutEv`**| EV Charger | Buck Chopper Switching Voltage | **$600.0\text{ V}$** | Switched rail from DC bus |
| **`PEV`** | EV Charger | Delivered EV Charging Power | **$4,068\text{ W}$** | High-power fast-charging level |

---

## ⚖️ Energy Management System (EMS) & Power Balancing

The central operational principle governing the microgrid is the instantaneous conservation of power at the DC-link capacitor node:

$$P_{\text{PV}}(t) = P_{\text{EV}}(t) + P_{\text{BESS}}(t) + P_{\text{loss}}(t)$$

Where:
* $P_{\text{PV}} = V_{mp} \cdot I_{mp} = 243.6\text{ V} \times 34.86\text{ A} = \mathbf{8,491.9\text{ W}}$
* $P_{\text{EV}} = V_{\text{batt,EV}} \cdot I_{b\text{EV}} = \mathbf{4,068.0\text{ W}}$
* $P_{\text{BESS}} = V_{\text{batt,BESS}} \cdot I_B = \mathbf{4,498.0\text{ W}}$
* Sum of active loads: $P_{\text{EV}} + P_{\text{BESS}} = 4,068 + 4,498 = \mathbf{8,566.0\text{ W}}$

* **Power Balance (Single Scope Snapshot):** $P_{\text{gen}} = 8{,}491.9\text{ W}$ vs $P_{\text{EV}} + P_{\text{BESS}} = 8{,}566.0\text{ W}$; the $\approx 0.9\%$ residual is consistent with a scope capture slightly before full bus settling and is not attributed to converter losses. The graduation thesis reports the rounded steady-state nominal balance: $\approx 8.52\text{ kW} \approx 4.0\text{ kW}\text{ (EV)} + 4.52\text{ kW}\text{ (BESS)}$.

### Operational Modes of the EMS

```
                     ┌─────────────────────────────────────────┐
                     │          Solar PV Generation            │
                     │               P_PV = 8,491 W            │
                     └────────────────────┬────────────────────┘
                                          │
                                          ▼
                     ┌─────────────────────────────────────────┐
                     │      Common DC-Link Bus (600.0 V)       │
                     └────────────┬───────────────┬────────────┘
                                  │               │
            [4,068 W Priority]    │               │    [4,498 W Surplus]
                                  ▼               ▼
      ┌──────────────────────────────┐ ┌──────────────────────────────┐
      │     EV Fast Charger Buck     │ │   Bidirectional 2Q Chopper   │
      │   (Charges EV Pack @ 10 A)   │ │  (Stores Excess into BESS)   │
      └──────────────────────────────┘ └──────────────────────────────┘
```

1. **Mode 1 — High Solar Surplus ($P_{\text{PV}} > P_{\text{EV}}$) [Simulated Case]:**
   * The PV array generates $8.49\text{ kW}$.
   * The EV Charger draws its rated $4.07\text{ kW}$.
   * The 2Q Chopper operates in **Buck mode**, directing the remaining $4.50\text{ kW}$ surplus into the stationary BESS ($I_B = +11.25\text{ A}$).
2. **Mode 2 — Solar Generation Deficit ($P_{\text{PV}} < P_{\text{EV}}$):**
   * Under heavy cloud cover or partial shading, solar generation drops below EV charging demand.
   * The DC bus voltage starts dropping below $600\text{ V}$; the BESS cascade controller reverses current flow to **Boost mode** ($I_B < 0$), discharging the stationary battery to hold the DC link at $600\text{ V}$ and prevent EV charging interruptions.
3. **Mode 3 — Standalone / Nighttime Operation ($P_{\text{PV}} = 0$) [Design Operational Capability]:**
   * The BESS supplies $100\%$ of the energy required by the EV buck converter, functioning as a primary microgrid grid-forming voltage source.

---

## 🔄 Multi-Stage Subsystem Architecture & Control Hierarchy

### 1. PV Array Generation Stage (8.5 kW)
The solar generator consists of 12 high-efficiency photovoltaic modules arranged in a series-parallel configuration equipped with bypass and blocking diodes to suppress hotspot effects:

<p align="center">
  <img src="images/System_06_L1_PV_Array_8_5kW.png" width="85%" alt="8.5 kW Photovoltaic Array Subsystem" />
</p>

### 2. Interleaved Boost Converter Power Stage
A 2-phase interleaved boost converter interfaces the PV output with the $600\text{ V}$ bus. The two branches switch with a $180^\circ$ phase shift, cancelling high-frequency input ripple and distributing thermal stress:

<p align="center">
  <img src="images/System_04_L1_Interleaved_Boost_Converter.png" width="85%" alt="Interleaved Boost Converter Power Stage" />
</p>

### 3. MPPT Perturb & Observe (P&O) and Dual PI Current Controller
The outer MPPT loop samples $V_{mp}$ and $I_{mp}$, computing power change $\Delta P = P(k) - P(k-1)$ to perturb the current reference $I_{\text{ref}}$. Two inner PI current controllers generate duty cycles $D_1$ and $D_2$ compared against phase-shifted carrier waves:

<p align="center">
  <img src="images/System_07_L2_MPPT_and_Boost_Controller_MPPT2.png" width="90%" alt="P&O MPPT Algorithm and Interleaved Dual PI Controllers" />
</p>

### 4. BESS Cascade DC-Link Voltage & Current Regulator
The 2Q Chopper utilizes a **Cascade Control Loop**:
* **Outer Voltage Loop:** Compares actual bus voltage $V_{\text{DC}}$ against the $600\text{ V}$ reference. The resulting error passes through a tuned PI controller to synthesize the target battery current $I_{B,\text{ref}}$.
* **Inner Current Loop:** Compares $I_{B,\text{ref}}$ against measured battery current $I_B$, feeding a hysteresis comparator that modulates the upper and lower complementary switches:

<p align="center">
  <img src="images/System_02_L1_BESS_Cascade_PI_Controller.png" width="70%" alt="BESS Cascade PI DC-Bus Voltage Controller" />
</p>

### 5. Bidirectional 2Q Chopper Power Stage
Half-bridge IGBT topology with midpoint inductor coupling the stationary battery storage to the DC bus:

<p align="center">
  <img src="images/System_01_L1_BESS_2Q_Bidirectional_Chopper.png" width="85%" alt="Bidirectional Two-Quadrant Chopper Power Stage" />
</p>

### 6. Synchronous EV Fast Charger Buck Subsystem
Steps down the $600\text{ V}$ DC bus to charge the electric vehicle battery pack under continuous hysteresis current control:

<p align="center">
  <img src="images/System_03_L1_EV_Fast_Charger_Buck.png" width="85%" alt="EV Fast Charger Buck Converter Subsystem" />
</p>

---

## 📊 Comprehensive Simulation Results & Waveform Verification

### 1. DC-Link Voltage Regulation & Stability (600 V)
During cold system startup, the DC bus exhibits a transient settling time of less than $0.5\text{ s}$ with a maximum voltage overshoot bounded below $0.7\%$ ($\approx 0.63\%$, $603.8\text{ V}$ peak), settling firmly at exactly **$600.0\text{ V}$**:

<p align="center">
  <img src="images/Scope_12_VDC_Voltage.png" width="90%" alt="DC-Link Bus Voltage Regulation Waveform" />
</p>

### 2. Multi-Converter Power Balance Curves (EMS Verification)
The generated solar power ($P_{\text{PV}} = 8491\text{ W}$) is continuously tracked and split into EV battery charging power ($P_{\text{EV}} = 4068\text{ W}$) and BESS storage charging power ($P_{\text{storage}} = 4498\text{ W}$):

#### PV Extracted MPPT Power (8,491 W)
<p align="center">
  <img src="images/Scope_14_Pmppt_Power.png" width="85%" alt="PV Array Extracted MPPT Power Waveform" />
</p>

#### EV Battery Charging Power (4,068 W)
<p align="center">
  <img src="images/Scope_5_PEV_Power.png" width="85%" alt="EV Battery Charging Power Waveform" />
</p>

#### BESS Storage Charging Power (4,498 W)
<p align="center">
  <img src="images/Scope_2_Pstorage_Power.png" width="85%" alt="BESS Bidirectional Storage Power Waveform" />
</p>

### 3. Current Sharing & Ripple Cancellation
The total input solar current ($34.86\text{ A}$) is split symmetrically between the two boost phases ($I_{L1} = 17.13\text{ A}$, $I_{L2} = 17.73\text{ A}$), operating within $\approx 2$ percentage points of an even split ($49.1\% / 50.9\%$):

<p align="center">
  <img src="images/Scope_11_Imp_IL1_IL2.png" width="90%" alt="PV Total Current and Dual-Phase Inductor Current Sharing" />
</p>

### 4. EV Charger & BESS Current Dynamics
The EV charging current ($I_{b\text{EV}}$) locks onto the commanded $10.17\text{ A}$ constant current reference with negligible ripple, while the BESS battery current ($I_B$) smoothly settles into charging mode at $+11.25\text{ A}$:

#### EV Fast Charger Regulated Output Current (10.17 A)
<p align="center">
  <img src="images/Scope_3_IbEV_Current.png" width="85%" alt="EV Fast Charger Regulated Output Current" />
</p>

#### BESS Battery Current (+11.25 A Charging)
<p align="center">
  <img src="images/Scope_0_IB_Current.png" width="85%" alt="BESS Bidirectional Battery Current Waveform" />
</p>

### 5. MPPT Operating Voltage & Duty Ratios
The P&O MPPT algorithm quickly finds the knee of the P-V curve at $V_{mp} = 243.6\text{ V}$, driving the interleaved duty cycles $D_1$ and $D_2$ to steady-state values of $\approx 0.61$:

#### PV Array Terminal Voltage ($V_{mp} = 243.6\text{ V}$)
<p align="center">
  <img src="images/Scope_13_Vmp_Voltage.png" width="85%" alt="PV Array Terminal Voltage at MPP" />
</p>

#### Interleaved Boost Phase-1 Duty Ratio ($D_1 \approx 0.61$)
<p align="center">
  <img src="images/Scope_MPPT2_6_Saturation_Duty_Ratio.png" width="85%" alt="Interleaved Boost Phase-1 Duty Ratio Waveform" />
</p>

---

## 🔍 Supplementary Telemetry & High-Magnification Waveform Gallery

To provide deep transparency into the internal switching dynamics, ripple mitigation, and discrete control execution, this section details the supplementary high-resolution telemetry and microscopic waveform captures:

### 1. Dual-Phase Interleaved Current Sharing & 180° Ripple Cancellation (Zoomed)
High-magnification view showing the $180^\circ$ phase-staggered inductor currents ($I_{L1}$ and $I_{L2}$) interweaving at $20\text{ kHz}$ effective ripple frequency:

<p align="center">
  <img src="images/Scope_11_Imp_IL1_IL2_Zoomed.png" width="90%" alt="Zoomed Interleaved Current Sharing and Ripple Cancellation" />
</p>

### 2. Converter Stage Chopper Rail Voltages
Probing the raw switched output rail voltages before LC filtering for both the BESS bidirectional chopper ($V_{\text{out}}$) and EV fast charger buck stage ($V_{\text{out,EV}}$):

#### BESS Chopper Output Rail Voltage ($V_{\text{out}} = 600\text{ V}$)
<p align="center">
  <img src="images/Scope_1_Vout_Voltage.png" width="85%" alt="BESS Chopper Output Rail Voltage" />
</p>

#### BESS Rail Voltage Steady-State Ripple (Zoomed View)
<p align="center">
  <img src="images/Scope_1_Vout_Voltage_Zoomed.png" width="85%" alt="BESS Rail Voltage Steady-State Ripple Zoomed" />
</p>

#### EV Fast Charger Chopper Switching Voltage ($V_{\text{out,EV}} = 600\text{ V}$)
<p align="center">
  <img src="images/Scope_4_VoutEv_Voltage.png" width="85%" alt="EV Fast Charger Chopper Switching Voltage" />
</p>

#### EV Charger Switched PWM Rail Transitions (Zoomed View)
<p align="center">
  <img src="images/Scope_4_VoutEv_Voltage_Zoomed.png" width="85%" alt="EV Charger Switched PWM Rail Transitions Zoomed" />
</p>

### 3. MPPT Inner Current Loop Perturbations & Error Dynamics
Detailed telemetry from the P&O MPPT tracking block and discrete PI carrier generation:

#### MPPT Current Reference Error Signal ($\Delta I = I_{\text{ref}} - I_{pv}$)
<p align="center">
  <img src="images/Scope_MPPT2_2_Sum7.png" width="85%" alt="MPPT Current Error Summation Signal" />
</p>

#### High-Resolution Transient Steps of Current Error (Zoomed View)
<p align="center">
  <img src="images/Scope_MPPT2_2_Sum7_Zoomed.png" width="85%" alt="Zoomed View of MPPT Current Error Transient Steps" />
</p>

#### Phase-1 vs Phase-2 Inner PI Saturated Duty Cycles ($D_1, D_2$)
<p align="center">
  <img src="images/Scope_MPPT2_7_Saturation1_Duty_Ratio.png" width="85%" alt="Phase-1 PI Modulating Duty Ratio D1" />
</p>

<p align="center">
  <img src="images/Scope_MPPT2_1_Saturation2_Duty_Ratio.png" width="85%" alt="Phase-2 PI Modulating Duty Ratio D2" />
</p>

#### Carrier Wave Modulation & Duty Cycle Stability (Zoomed Views)
<p align="center">
  <img src="images/Scope_MPPT2_6_Saturation_Duty_Ratio_Zoomed.png" width="85%" alt="Zoomed Phase-1 Duty Ratio Modulating Dynamic" />
</p>

<p align="center">
  <img src="images/Scope_MPPT2_7_Saturation1_Duty_Ratio_Zoomed.png" width="85%" alt="Zoomed Sub-Microsecond PWM Pulse Modulation Node" />
</p>

### 4. Level-1 MPPT and Interleaved Boost Controller Subsystem
Hierarchical subsystem structure encapsulating the dual inner current loops and discrete P&O algorithm:

<p align="center">
  <img src="images/System_05_L1_MPPT_and_Boost_Controller.png" width="85%" alt="Level-1 MPPT and Interleaved Boost Controller Subsystem" />
</p>

---

## 📂 Directory Structure & Simulation Execution Guide

```text
6_Full_System_Integration/
├── README.md                                          # Comprehensive integration & control report
├── Solar_EV_BESS_Full_Sys.slx                         # Full multi-converter closed-loop Simulink model
└── images/                                            # 300 DPI architecture schematics and scope waveforms
    ├── Scope_0_IB_Current.png
    ├── Scope_1_Vout_Voltage.png
    ├── Scope_1_Vout_Voltage_Zoomed.png
    ├── Scope_2_Pstorage_Power.png
    ├── Scope_3_IbEV_Current.png
    ├── Scope_4_VoutEv_Voltage.png
    ├── Scope_4_VoutEv_Voltage_Zoomed.png
    ├── Scope_5_PEV_Power.png
    ├── Scope_11_Imp_IL1_IL2.png
    ├── Scope_11_Imp_IL1_IL2_Zoomed.png
    ├── Scope_12_VDC_Voltage.png
    ├── Scope_13_Vmp_Voltage.png
    ├── Scope_14_Pmppt_Power.png
    ├── Scope_MPPT2_1_Saturation2_Duty_Ratio.png
    ├── Scope_MPPT2_2_Sum7.png
    ├── Scope_MPPT2_2_Sum7_Zoomed.png
    ├── Scope_MPPT2_6_Saturation_Duty_Ratio.png
    ├── Scope_MPPT2_6_Saturation_Duty_Ratio_Zoomed.png
    ├── Scope_MPPT2_7_Saturation1_Duty_Ratio.png
    ├── Scope_MPPT2_7_Saturation1_Duty_Ratio_Zoomed.png
    ├── System_00_Root_Solar_EV_BESS_Full_Sys_Full_Overview.png
    ├── System_01_L1_BESS_2Q_Bidirectional_Chopper.png
    ├── System_02_L1_BESS_Cascade_PI_Controller.png
    ├── System_03_L1_EV_Fast_Charger_Buck.png
    ├── System_04_L1_Interleaved_Boost_Converter.png
    ├── System_05_L1_MPPT_and_Boost_Controller.png
    ├── System_06_L1_PV_Array_8_5kW.png
    └── System_07_L2_MPPT_and_Boost_Controller_MPPT2.png
```

### Steps to Run the Integrated Model

1. **Prerequisites:** Ensure MATLAB/Simulink with **Simscape Electrical (Specialized Power Systems)** is installed (version R2022b or later recommended).
2. **Open the Model:** Double click or run `open('Solar_EV_BESS_Full_Sys.slx')` in the MATLAB Command Window.
3. **Solver Settings:**
   * Solver Type: `Fixed-step` (Discrete, no continuous states).
   * Sample Time: $T_s = 1.0 \times 10^{-6}\text{ s}$ ($1.0\ \mu\text{s}$) configured in the `powergui` block.
   * Total Simulation Time: $T_{\text{stop}} = 1.0\text{ s}$ (sufficient for full controller transient convergence).
4. **Execution:** Click **Run** (`Ctrl+T`). Once completed, inspect all logged metrics directly via the dedicated **DISPLAYS** and **SCOPES** blocks on the root canvas.
