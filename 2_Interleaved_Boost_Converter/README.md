# ⚡ Two-Phase Interleaved Boost Converter & Full-Scale Sizing

This directory documents the design, analytical sizing, mathematical ripple modeling, and MATLAB/Simulink simulations for the **Two-Phase Interleaved Boost Converter** stage. Within our Solar-Powered EV Charging Station, this converter interfaces the photovoltaic array with the regulated 600V DC-Link bus, providing high step-up efficiency while minimizing input current ripple.

The repository captures our complete engineering design progression: from baseline open-loop prototyping to decoupled control tuning via Thevenin equivalents, culminating in a fully closed-loop multi-phase converter.

---

## 📐 Full-Scale PV Array Sizing & Design Specifications

To ensure the charging station delivers sufficient energy without overloading the battery storage, the system was sized based on daily solar insolation data and load requirements as documented in Section 5.1 of the project thesis:

### Sizing Methodology:
* **Target Daily Energy Delivery:** `E_daily = 42.5 kWh/day`
* **Average Peak Sun Hours (Alexandria, Egypt):** `T_peak ≈ 5 hours`
* **Required PV Array Nominal Power:**
  ```text
  P_PV = E_daily / T_peak = 42.5 kWh / 5 h = 8500 W (8.5 kW)
  ```
* **Module Selection:** High-efficiency monocrystalline modules (`P_module = 710 W`, `V_m = 40.9 V`, `I_m = 17.36 A`).
* **Total Module Count:**
  ```text
  N_modules = 8500 W / 710 W ≈ 12 Modules (Distributed across strings)
  ```

### Overall Converter Parameter Specifications:

| Parameter | Symbol | Value | Design Context |
| :--- | :---: | :---: | :--- |
| **Nominal Input Power** | `P_in` | 8.5 kW | Total array peak generation |
| **Nominal Array Voltage**| `V_in` | ~245.4 V | 6 series modules per string (N_s = 6) |
| **Regulated Output Voltage**| `V_DC` | 600.0 V | Standard microgrid DC-Link potential |
| **Nominal Voltage Gain** | `M_v` | 2.44 | Boost ratio (V_o / V_in) |
| **Switching Frequency** | `f_sw` | 10 kHz | Per-phase switching rate |
| **Effective Ripple Frequency**| `f_eff` | 20 kHz | Interleaved cancellation rate (2 × f_sw) |
| **Phase Displacement** | `θ` | 180° | Interleaved PWM gate displacement |

---

## 🧠 Ripple Cancellation Physics & Mathematical Modeling

The central engineering justification for selecting an interleaved topology over a single-switch boost converter is **input current ripple cancellation** and reduced thermal stress on passive components.

### Mathematical Derivation:
In a conventional single-phase boost converter, the inductor current ripple is:
```text
ΔI_L(single) = (V_in · D) / (L · f_sw)
```

In a **two-phase interleaved topology**, the two identical inductors (`L1 = L2 = L`) operate in parallel with gating pulses shifted by a half-period (`T_sw / 2 = 180°`). The total input current drawn from the PV array is the instantaneous sum of both branch currents:
```text
i_in(t) = i_L1(t) + i_L2(t)
```

Applying superposition over switching intervals yields the net input current ripple (`ΔI_in`):
```text
ΔI_in = [V_o / (L · f_sw)] · D · (1 - 2D)        for D ≤ 0.5

ΔI_in = [V_o / (L · f_sw)] · (2D - 1) · (1 - D)    for D > 0.5
```

### Key Engineering Deductions:
1. **Zero Theoretical Ripple at D = 0.5:** At a 50% duty cycle, the rising slope of `i_L1` perfectly cancels the falling slope of `i_L2`, driving net input ripple to zero (`ΔI_in = 0`).
2. **>50% Ripple Reduction:** Across all practical operational duty cycles (`0.2 < D < 0.8`), the input current ripple is attenuated by over 50% compared to a single-phase converter of equivalent power rating.
3. **Capacitor Longevity:** Reducing input current ripple dramatically suppresses high-frequency heating across the input filter capacitors, directly prolonging PV module and capacitor operating life.

---

## 🏛️ Architectural Comparison: Central vs. String Topologies

Our research explored two primary distribution strategies for deploying the interleaved converter stage:

* **Centralized Interleaved Topology:** Connects all 12 modules (2 parallel strings of 6 modules) to a single high-capacity multi-phase converter. Minimizes BOM component count, reduces control complexity, and optimizes peak conversion efficiency under uniform sunlight.
* **String Interleaved Topology:** Employs modular converters dedicated to individual strings. Provides superior energy yield under non-uniform shading and independent MPPT granularity, but requires additional sensing hardware.

---

## 📂 Engineering Progression & File Manifest

Our design process is captured across five sequential models:

### Phase 1: Open-Loop Baseline Validation
* **`Boost_Source_Open_Loop.slx`**: Validates basic step-up ratio and steady-state operating points on a single-phase inductor branch.
* **`Interleaved_Boost_Source_Open_Loop.slx`**: Implements parallel branches with 180° phase-shifted gate pulses to visually confirm natural ripple cancellation prior to closing the feedback loop.

### Phase 2: Decoupled Control Tuning via Thevenin Equivalent
* **`Boost_Thevenin_Equivalent.slx`**: Replaces the non-linear PV array with a parameterized Thevenin circuit (Controlled Current Source + DC Voltage Source), allowing analytical tuning of voltage/current PI parameters without solar non-linearities.
* **`Interleaved_Boost_Thevenin_Equivalent.slx`**: Applies the tuned multi-loop architecture to the interleaved topology.

### Phase 3: Final Integrated System
* **`Interleaved_Boost_Thevenin_Full_Control_ForDesign.slx`**: The production-level model featuring cascaded inner current and outer voltage PI loops, anti-windup clamping, and real-time PWM generation.

---

## 🏗️ System Architecture & Visual Verification

**Integrated Interleaved Model (`Interleaved_Boost_Thevenin_Full_Control_ForDesign.slx`):**  
[images/interleaved_system_model.png]

---

## 📊 Experimental Simulation Results

### 1. Verification of Ripple Cancellation (The Interleaving Advantage)
The scope captures the individual inductor currents (`I_L1` in red, `I_L2` in blue) exhibiting expected ripple. Their instantaneous summation (`I_total` in cyan) demonstrates remarkable ripple attenuation, confirming our mathematical derivations.

| Input Current Ripple Summation | Zoomed-in Waveform Details |
| :---: | :---: |
| [images/ripple_cancellation.png] | [images/ripple_cancellation_zoomed.png] |

### 2. Tight Output DC-Link Regulation (600V Target)
The closed-loop multi-loop control maintains rock-solid regulation of the output voltage and current under dynamic load steps.

| Regulated Output Waveforms | Zoomed Steady-State Output |
| :---: | :---: |
| [images/interleaved_output_scope.png] | [images/interleaved_output_scope_zoomed.png] |

---

## 🔧 How to Run
1. Open MATLAB and navigate to `2_Interleaved_Boost_Converter/`.
2. To inspect the progressive design evolution, run any model from Phase 1 through Phase 3.
3. Open the scope blocks to evaluate current ripple cancellation and output voltage settling times.