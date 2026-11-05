# ☀️ Solar-Powered EV Charging Station with Battery Energy Storage (BESS)

![MATLAB/Simulink](https://img.shields.io/badge/MATLAB-Simulink_R2024b-0076A8?logo=mathworks&logoColor=white)
![Simscape Electrical](https://img.shields.io/badge/Simscape-Specialized_Power_Systems-blue)
![Altium Designer](https://img.shields.io/badge/Altium_Designer-24-A6192E?logo=altiumdesigner&logoColor=white)
![Control Systems](https://img.shields.io/badge/Control-Closed--Loop_PI_%26_Cascade-success)
![Power Electronics](https://img.shields.io/badge/Domain-Power_Electronics_%26_Microgrid-critical)
![Institution](https://img.shields.io/badge/Alexandria_University-Faculty_of_Engineering-003366)

> **B.Sc. Graduation Project 2026** — Electrical Power and Machines Department, Faculty of Engineering, Alexandria University.  
> **Thesis Title:** *Integration of Photovoltaic Systems and Energy Storage for Electric Vehicle Charging Station*.  
> **Academic Supervisor:** Prof. Dr. Ahmed Abbas El Serougi

[![LinkedIn](https://img.shields.io/badge/LinkedIn-Abd_El--Rhman_Saad-0A66C2?logo=linkedin&logoColor=white)](https://linkedin.com/in/Abd-El-Rhman-Saad)
[![GitHub](https://img.shields.io/badge/GitHub-Abd--El--Rhman--Saad-181717?logo=github&logoColor=white)](https://github.com/Abd-El-Rhman-Saad)
[![Email](https://img.shields.io/badge/Email-abdelrhmanmsaad22@gmail.com-D14836?logo=gmail&logoColor=white)](mailto:abdelrhmanmsaad22@gmail.com)

---

## 🏛️ Master Architecture Overview

The repository documents the complete end-to-end engineering lifecycle of a **standalone, off-grid solar-powered Electric Vehicle (EV) fast-charging microgrid**. The system coordinates a high-efficiency Photovoltaic (PV) array, an Interleaved Boost Converter with Perturb & Observe (P&O) MPPT, a centralized regulated $600.0\text{ V}$ DC-Link bus, a bidirectional Class-C Two-Quadrant Chopper interfacing stationary Battery Energy Storage (BESS), and a fast-acting Synchronous Buck Converter charging an EV battery pack.

<p align="center">
  <img src="6_Full_System_Integration/images/System_00_Root_Solar_EV_BESS_Full_Sys_Full_Overview.png" width="95%" alt="Complete Microgrid Closed-Loop Architecture" />
</p>

---

## 📌 Executive Summary & Key Engineering Specifications

Grid-independent EV charging stations face the dual challenges of intermittent solar irradiance and dynamic, high-power vehicle load steps. This project bridges rigorous **mathematical modeling** (state-space averaging, non-ideal parasitic compensation, discrete bilinear Z-domain transformation), high-fidelity **MATLAB/Simulink digital twins**, and practical **hardware prototyping** (Altium Designer multi-layer PCBs, ATmega328P embedded Timer 1 firmware, and laboratory bench validation).

### Master System Specifications

| Technical Metric | Full-Scale Utility Rating | Scaled Hardware Prototype | Engineering Verification Status |
| :--- | :---: | :---: | :--- |
| **Common DC-Link Bus** | **$600.0\text{ V DC}$** | $12.0\text{ V} - 24.0\text{ V}$ | Regulated via BESS cascade control (settles firmly at $600.0\text{ V}$) |
| **Solar PV Generation** | **$\approx 8.5\text{ kW}$** ($8,491\text{ W}$ Scope / $\approx 8.52\text{ kW}$ Thesis) | 12V / 10W Emulated Source | P&O MPPT tracking knee of P-V curve at $V_{mp} = 243.6\text{ V}$ |
| **Interleaved Boost Stage** | Dual-Phase, $180^\circ$ Phase Shift | Dual $2.0\text{ mH}$ E-Core Power Chokes | $49.1\% / 50.9\%$ current sharing; cancels $20\text{ kHz}$ input current ripple |
| **BESS Energy Buffer** | **$3.48\text{ kW}$** Deficit / $+4.50\text{ kW}$ Surplus | $7.4\text{ V}$ ($2.55\text{ Ah}$) 2S Li-ion | Synchronous bidirectional power flow (Buck charge / Boost discharge) |
| **EV Charging Power** | **$50\text{ kW}$** Utility Sizing / **$4.07\text{ kW}$** Fast Track (Surplus) / **$12.0\text{ kW}$** Deficit ($3.48\text{ kW}$ BESS support) | $3.7\text{ V} / 0.74\text{ A}$ Prototype | Dual-loop automatic CC→CV transition |
| **Transient Settling Time** | **$< 0.5\text{ s}$** | $< 0.4\text{ s}$ | Voltage overshoot bounded below $0.7\%$ ($\approx 0.63\%$, $603.8\text{ V}$ peak) |
| **Switching Frequency** | $10.0\text{ kHz}$ to $20\text{ kHz}$ | **$10.0\text{ kHz}$** | ATmega328P Timer 1 Phase-Correct PWM ($1\ \mu\text{s}$ dead-time) |

> **Engineering Note on Simulation vs. Thesis Reporting:** Simulation values are read directly from the full microgrid model scopes: $V_{mp} \cdot I_{mp} = 243.6\text{ V} \times 34.86\text{ A} = 8,491.9\text{ W}$ generation, $P_{\text{EV}} = 4,068.0\text{ W}$, and $P_{\text{storage}} = +4,498.0\text{ W}$ (surplus charging). In the deficit regime, EV charging draws $12.0\text{ kW}$ with $3.48\text{ kW}$ injected by the BESS. The $50\text{ kW}$ rating represents the utility-scale open-loop fast-charger design stage in Module 4. The project thesis text reports rounded nominal figures of $\approx 8.52\text{ kW}$ solar capacity, $\approx 4\text{ kW}$ EV charging load, and $\approx 3.48\text{ kW}$ BESS deficit support.

---

## ⚖️ Energy Management System (EMS) & Power Balancing

The operational nucleus of the microgrid is the instantaneous conservation of energy at the common DC-link capacitor node:

$$P_{\text{PV}}(t) = P_{\text{EV}}(t) + P_{\text{BESS}}(t) + P_{\text{loss}}(t)$$

Under Kirchhoff's Current Law (KCL) at the central $600.0\text{ V}$ bus node:

$$C_{\text{dc}} \frac{d V_{\text{DC}}}{d t} = I_{\text{boost,out}}(t) - I_{\text{chopper,in}}(t) - I_{\text{buck,in}}(t) \approx 0$$

```text
       ┌─────────────────────────────────────────┐
       │           Solar PV Generation           │
       │               P_PV = 8,491 W            │
       └────────────────────┬────────────────────┘
                            │
                            ▼
       ┌─────────────────────────────────────────┐
       │      Common DC-Link Bus (600.0 V)       │
       └────────────┬───────────────┬────────────┘
                    │               │
     Surplus Energy │               │ Deficit Support
       (+4,498 W)   ▼               ▼ (-3.48 kW max)
       ┌────────────────────────┐  ┌────────────────────────┐
       │   BESS 2Q Converter    │  │  EV Fast Buck Charger  │
       │ (7.4V / 2.55Ah Li-ion) │  │  (CC/CV Mode: 4,068 W) │
       └────────────────────────┘  └────────────────────────┘
```

### Dynamic EMS Operational Regimes & Power Flow (Operational Concept / Design Intent)

The supervisory EMS architecture governs power balance across four conceptual operating regimes (thresholds illustrative; primary solar surplus and deficit modes validated in closed-loop simulation):

| Operational Regime | Generation vs. Load Condition | Converter Interactions & Power Flow Direction | DC-Link Voltage Regulation Mechanism |
| :--- | :--- | :--- | :--- |
| **Regime 1: Solar Surplus** | $P_{\text{PV}} > P_{\text{EV}}$<br>($G \ge 800\text{ W/m}^2$, $SOC < 95\%$) | • **PV:** Maximum Power Point Tracking ($8,491\text{ W}$)<br>• **EV:** Regulated Fast Charging ($4,068\text{ W}$)<br>• **BESS:** **Buck Charging** absorbs $+4,498\text{ W}$ surplus | BESS outer PI voltage controller modulates high-side switch $Q_1$, clamping bus tightly at $600.0\text{ V}$. |
| **Regime 2: Cloud Deficit** | $P_{\text{PV}} < P_{\text{EV}}$<br>($G \le 400\text{ W/m}^2$, $SOC > 20\%$) | • **PV:** Degraded harvesting ($P_{\text{PV}} < 4\text{ kW}$)<br>• **EV:** Uninterrupted constant current charging<br>• **BESS:** **Boost Discharging** injects up to $3.48\text{ kW}$ | BESS controller rapidly reverses current flow ($I_B < 0$), preventing DC bus voltage sag through bidirectional buffering. |
| **Regime 3: Autonomous Night** | $P_{\text{PV}} = 0$<br>(Night operation, $SOC > 20\%$) | • **PV:** Zero generation (Boost converter idle)<br>• **EV:** Sustained charging supported entirely by BESS<br>• **BESS:** Sole voltage-forming source ($P_{\text{BESS}} = -P_{\text{EV}}$) | BESS acts as primary grid-forming voltage anchor, maintaining $600.0\text{ V}$ until battery hits $20\%$ DoD limit. |
| **Regime 4: Full Charge Curtailment** | $P_{\text{PV}} > P_{\text{EV}}$<br>($SOC_{\text{BESS}} \ge 95\%$, $I_{\text{EV}} = 0$) | • **PV:** Deliberate MPPT power curtailment ($V_{\text{pv}} \to V_{\text{oc}}$)<br>• **EV:** Disconnected or fully charged<br>• **BESS:** Float mode ($I_B \approx 0\text{ A}$) to protect cell chemistry | MPPT sweep moves away from $V_{mp}$ to match auxiliary station parasitic consumption, preventing DC bus overvoltage. |

### Quantitative Power Equilibrium Verification

Scope-snapshot power balance (single capture):
* **Total Instantaneous Generation ($P_{\text{gen}}$):** $V_{mp} \cdot I_{mp} = 243.6\text{ V} \times 34.86\text{ A} = \mathbf{8,491.9\text{ W}}$
* **Total Instantaneous Demand ($P_{\text{load}}$):** $P_{\text{EV}} + P_{\text{BESS}} = 4,068.0\text{ W} + 4,498.0\text{ W} = \mathbf{8,566.0\text{ W}}$
* **Power Balance (Single Scope Snapshot):** $P_{\text{gen}} = 8{,}491.9\text{ W}$ vs $P_{\text{EV}} + P_{\text{BESS}} = 8{,}566.0\text{ W}$; the $\approx 0.9\%$ residual is consistent with a scope capture slightly before full bus settling and is not attributed to converter losses. The graduation thesis reports the rounded steady-state nominal balance: $\approx 8.52\text{ kW} \approx 4.0\text{ kW}\text{ (EV)} + 4.52\text{ kW}\text{ (BESS)}$.

---

## 🚀 Interactive Modular Repository Tour

The repository is structured into six modular engineering directories matching the project's physical subsystems:

| Directory | Core Technical Scope | Primary Simulation Model | Key Hardware & Design Assets |
| :--- | :--- | :--- | :--- |
| [**`1_PV_Modeling_and_MPPT`**](1_PV_Modeling_and_MPPT/) | Single-diode PV modeling, P&O algorithm, Incremental Conductance benchmark, and partial shading check. | `MPPT_PO1.slx`<br>`pv_curves.slx` | `mppt_po.c` (ANSI C firmware)<br>13 STC parameter baseline |
| [**`2_Interleaved_Boost_Converter`**](2_Interleaved_Boost_Converter/) | 2-phase interleaved boost converter sizing, input ripple cancellation ($180^\circ$ phase shift), and central vs. string comparison. | `Interleaved_Boost_Thevenin_Equivalent.slx`<br>`Interleaved_Boost_Thevenin_Full_Control_ForDesign.slx`<br>`Interleaved_Boost_Source_Open_Loop.slx` | Mathematical ripple proofs ($D \le 0.5$ and $D > 0.5$)<br>600 V / 8.5 kW sizing |
| [**`3_BESS_and_2Q_Chopper`**](3_BESS_and_2Q_Chopper/) | Class-C bidirectional chopper, cascaded voltage PI + hysteresis current control, and bilinear Tustin discretization. | `BESS_2Q_All_Cases_Pro.slx`<br>`chopper_actual_rating.slx` | Embedded MATLAB Function source code<br>6-case benchmark matrix |
| [**`4_EV_Buck_Charger`**](4_EV_Buck_Charger/) | Synchronous step-down buck charger, non-ideal parasitic compensation ($D_u \approx 39.75\%$), and automated CC-to-CV transition. | `Buck_Closed_Loop_CCCV_Charger.slx`<br>`Buck_Prototype_Current_Control.slx` | Non-ideal volt-second derivation<br>Dual-loop state machine |
| [**`5_Hardware_and_PCB_Design`**](5_Hardware_and_PCB_Design/) | Complete Altium Designer projects, Star Grounding layout, ATmega328P Timer 1 C++ firmware, and bench multimeter testing. | `Two_Quadrant_Chopper.zip`<br>`Buck_Converter.zip` | 10 kHz Phase-Correct PWM code<br>12 bench verification photos |
| [**`6_Full_System_Integration`**](6_Full_System_Integration/) | Master closed-loop microgrid coordination, EMS power balance verification, and full-system telemetry scopes. | `Solar_EV_BESS_Full_Sys.slx` | 28 level-1/2 schematics & scopes<br>Instantaneous power displays |
| [**`Firmware`**](Firmware/) | Thesis firmware listings (§6.1.5, §6.2.5), MATLAB controller functions (§5.1.3.1, §5.5.2), and bench telemetry sketch. | `Thesis_Listing_6.1_MPPT_Interleaved_Boost.ino`<br>`Thesis_Listing_6.3_Buck_FixedPoint_PI_ISR.ino` | Verified C++ Arduino sketches<br>Embedded MATLAB Functions |

---

## 🔬 Hardware Prototyping & Experimental Laboratory Validation

Beyond numerical simulation, functional converter prototypes were designed in **Altium Designer**, fabricated, and experimentally verified on the laboratory bench:

### Laboratory Instrumentation & Semiconductor Baseline

* **Switching Stage:** IRFP260N Power MOSFETs ($200\text{ V}$, $50\text{ A}$, $R_{\text{DS(on)}} = 0.04\ \Omega$) mounted on extruded aluminum heatsinks.
* **Gate Drive Circuitry:** IR2111 half-bridge gate drivers with $1.0\ \mu\text{s}$ internal dead-time paired with 6N139 optocouplers ($5000\text{ V}_{\text{RMS}}$ isolation).
* **Feedback Sensing:** ACS712-05B linear Hall-effect current sensor ($185\text{ mV/A}$ sensitivity) with hardware RC low-pass filtering.
* **Digital Signal Generation:** Bare-metal ATmega328P 16-bit Timer 1 configured in Phase-Correct PWM mode, generating a clean $10.0\text{ kHz}$ carrier ($\text{TOP} = 800$).

### Experimental Closed-Loop Current Tracking Results

Bench current tracking combines thesis Table 6.1 ($0.20\text{ A}$, $0.30\text{ A}$, and $0.40\text{ A}$ nominal baseline) with extended laboratory multimeter photos in [`5_Hardware_and_PCB_Design/images/`](5_Hardware_and_PCB_Design/images/) ($0.30\text{ A}$, $0.40\text{ A}$, $0.50\text{ A}$, and $0.60\text{ A}$). The $0.40\text{ A}$ photo reads $0.38\text{ A}$, bounding the worst-case error across all measured setpoints at $-5.0\%$:

| Current Setpoint ($I_{\text{ref}}$) | Multimeter Reading ($I_{\text{meas}}$) | Absolute Error ($\Delta I$) | Relative Error | Source / Verification Reference |
| :---: | :---: | :---: | :---: | :--- |
| **$0.20\text{ A}$** | **$0.19\text{ A}$** | $-0.01\text{ A}$ | **$-5.0\%$** | Thesis Table 6.1 baseline |
| **$0.30\text{ A}$** | **$0.29\text{ A}$** | $-0.01\text{ A}$ | **$-3.3\%$** | Thesis Table 6.1 & bench photo (`reading_0_3A.png`) |
| **$0.40\text{ A}$** | **$0.38\text{ A}$** | $-0.02\text{ A}$ | **$-5.0\%$** | Bench photo (`reading_0_4A.png`); nominal setpoint |
| **$0.50\text{ A}$** | **$0.48\text{ A}$** | $-0.02\text{ A}$ | **$-4.0\%$** | Bench photo (`reading_0_5A.png`) |
| **$0.60\text{ A}$** | **$0.58\text{ A}$** | $-0.02\text{ A}$ | **$-3.3\%$** | Bench photo (`reading_0_6A.png`) |

<p align="center">
  <img src="5_Hardware_and_PCB_Design/images/all_hardware_system.png" width="90%" alt="Laboratory Hardware Experimental Bench Setup" />
</p>

---

## 📘 Project Documentation & Academic Thesis

The comprehensive academic documentation for this graduation project is compiled and archived in the [`Docs/`](Docs/) directory:

* **Complete Academic Book (PDF):** [`Docs/Final Graduation Project Documentation - 2026.pdf`](Docs/Final%20Graduation%20Project%20Documentation%20-%202026.pdf)
* **Full LaTeX Source Code:** [`Docs/LaTeX_Source/`](Docs/LaTeX_Source/) (contains all chapter `.tex` sources, vector figures, and BibTeX citations).

---

## 💻 Quickstart & Simulation Execution Guide

### Software & Toolchain Prerequisites

1. **MATLAB / Simulink:** Version R2024b (or later) with **Simscape Electrical** (Specialized Power Systems).
2. **PCB Design & EDA:** **Altium Designer 24** (or standard Gerber viewer / CAMtastic for manufacturing outputs).
3. **Embedded Firmware Development:** Arduino IDE 2.x or PlatformIO with AVR toolchain for ATmega328P.

### Steps to Run the Integrated System

1. Clone the repository to your local workstation:
   ```bash
   git clone https://github.com/Abd-El-Rhman-Saad/solar-ev-charging-station-bess.git
   cd solar-ev-charging-station-bess
   ```
2. Launch MATLAB and navigate to the integrated model directory:
   ```matlab
   cd('6_Full_System_Integration')
   open('Solar_EV_BESS_Full_Sys.slx')
   ```
3. Ensure the solver configuration in the `powergui` block is set to **Discrete** ($T_s = 1.0\ \mu\text{s}$).
4. Run the simulation (`Ctrl+T`). Once completed, inspect all instantaneous powers, voltages, and currents via the root canvas **DISPLAYS** and **SCOPES** blocks.

---

## 👥 Engineering Team & Academic Supervision

Developed as a Senior B.Sc. Graduation Project by a dedicated team of electrical engineers:

* **Abd El-Rhman Muhammad Saad**
* **Nour El-Deen Mohamed Fathy**
* **Mai Nagah Ali**
* **Mariam Mamdouh Ibrahim**
* **Rewan Amr Mahmoud**
* **Rwan Mohamed Abd-Elmokhtar**
* **Yasmeen Salah Khobeez**
* **Youssef Ebrahim Abd El-Sattar**

**Academic Supervisor:**  
**Prof. Dr. Ahmed Abbas El Serougi** — Professor of Electrical Power and Machines, Faculty of Engineering, Alexandria University.

---

### 👤 Individual Contribution & Technical Scope

**Abd El-Rhman Muhammad Saad — Hardware Design, Review & Bench Validation:**

| Board | Role |
| :--- | :--- |
| **Two-Quadrant Chopper** | Sole designer: schematic and PCB layout in Altium Designer; also assembled, soldered, and bench-tested the board. |
| **Interleaved Boost Converter** | Reviewed the sub-team's design and corrected errors; after fabrication, reworked the printed board by hand to resolve a layout error and tight component spacing: tracks were cut and the connections were re-made with jumper wires and trimmed component leads, some on the same layer and some bridged between the two copper layers. |
| **EV Buck Charger** | Reviewed the design and advised on it; assembled and soldered the board. |

* **Firmware:** Debugged and reviewed the embedded control firmware (written primarily by teammate Mai Nagah Ali), identifying errors and applying fixes during bench verification.
* **Simulation:** Built an independent Simulink model of the BESS / two-quadrant chopper system; the team cross-reviewed each other's models and converged on the final integrated model.

#### 🛠️ Key Technical Challenges Addressed by Abd El-Rhman:

1. **Optocoupler Logic Inversion (Design Fact):**
   * *Circuit Behavior:* The 6N139 common-emitter optocoupler configuration inverts microcontroller logic (logic LOW turns driver input HIGH).
   * *Implementation Fact:* The gate driver stage is inverted, so the control firmware computes the effective duty cycle as $D_{\text{effective}} = \frac{\text{TOP} - \text{OCR1A}}{\text{TOP}}$, with `OCR1A` constrained within `[50, TOP_PWM - 50]` to bound effective duty between $\approx 6\% \text{ and } 94\%$.
2. **Ground Isolation & Sensor Noise Protection (Design Architecture):**
   * *Design Context:* Rapid $di/dt$ switching pulses from the IRFP260N MOSFETs can couple high-frequency switching transients into sensitive analog measurement lines.
   * *Engineering Implementation:* Architected a dedicated Star Grounding scheme segregating Power Ground (PGND) from Signal Ground (AGND) with a single-point connection at the bulk DC link capacitor.
3. **Switching Frequency & Audible Whine:**
   * *Observation:* An audible whine was observed from the 2.0 mH E-core choke under the Arduino default PWM frequency ($\approx 490\text{ Hz}$).
   * *Resolution:* The whine disappeared completely on the bench after the firmware was set to the $10.0\text{ kHz}$ design frequency.

#### 📊 Practical Competency & Artifact Mapping:

| Core Competency | Industrial Tools & Standards Applied | Verified Artifact in Repository |
| :--- | :--- | :--- |
| **Power PCB Design & Rework** | Altium Designer 24, Star Grounding, Manual PCB Rework | [`5_Hardware_and_PCB_Design/`](5_Hardware_and_PCB_Design/) |
| **Firmware Review & Bench Debugging** | Embedded C++, AVR Timer 1 Registers, Serial Telemetry | [`5_Hardware_and_PCB_Design/README.md`](5_Hardware_and_PCB_Design/README.md) |
| **Closed-Loop Converter Modeling** | Simscape Electrical, Cascade PI, Bidirectional Chopper | [`3_BESS_and_2Q_Chopper/`](3_BESS_and_2Q_Chopper/) |
