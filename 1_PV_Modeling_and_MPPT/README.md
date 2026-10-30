# ☀️ PV System Modeling & Advanced MPPT Implementation

This repository directory contains the mathematical modeling of Photovoltaic (PV) array characteristics and the progressive implementation of Maximum Power Point Tracking (MPPT) algorithms—progressing from baseline **Perturb & Observe (P&O)** logic to advanced **Global Scanning (Forced Sweep)** for partial shading mitigation.

---

## 📂 Folder Structure & File Manifest

* **`pv_curves.slx`**: Mathematical single-diode modeling of the PV cell to extract non-linear I-V and P-V characteristic curves.
* **`MPPT_PO1.slx`**: Dedicated Simulink logic block implementing the primary **Perturb and Observe (P&O)** tracking algorithm.
* **`mppt_po.c`**: Embedded C firmware source code implementing the P&O logic for microcontroller execution.
* **`Incremental_conductance.slx`**: Comparative simulation benchmark evaluating the incremental conductance method against P&O.
* **`Central_Boost_MPPT_with_check.slx`**: The advanced **Global Scanning MPPT (Forced Sweep)** model designed to eliminate local-maximum trapping under Partial Shading Conditions (PSC).

---

## 1️⃣ PV Array Modeling & Physical Parameters (`pv_curves.slx`)

To ensure simulation fidelity without relying on generic pre-packaged libraries, we formulated an analytical model based on the single-diode equivalent circuit ($R_s - R_p$ configuration).

### Standard Test Conditions (STC) & Module Baseline:

The model parameters were configured to match our project hardware baseline:

| Parameter | Symbol | Value | Physical Significance |
| :--- | :---: | :---: | :--- |
| **Nominal Irradiance** | $I_{ref}$ | $1000\text{ W/m}^2$ | Standard solar benchmark |
| **Cell Temperature** | $T_{ref}$ | $318\text{ K}$ ($45^\circ\text{C}$) | Operational cell temperature |
| **Silicon Bandgap Energy**| $E_g$ | $1.12\text{ eV}$ | Standard bandgap potential |
| **Series Cells per Module**| $N_s$ | $36$ | Internal series cell count |
| **Maximum Power Point** | $P_{max}$ | $192.52\text{ W}$ | Peak extractable power |
| **Voltage at MPP** | $V_{mpp}$ | $36.0\text{ V}$ | Voltage at maximum power point |
| **Current at MPP** | $I_{mpp}$ | $5.32\text{ A}$ | Current at maximum power point |
| **Open-Circuit Voltage** | $V_{oc}$ | $229.4\text{ V}$ | Array series voltage ($22.94\text{ V} \times 10$) |
| **Short-Circuit Current** | $I_{sc}$ | $5.56\text{ A}$ | Array short-circuit current |
| **Series Resistance** | $R_s$ | $0.145\ \Omega$ | Ohmic losses in cell contacts |
| **Parallel Resistance** | $R_p$ | $300\ \Omega$ | Leakage current through p-n junction |
| **Temp. Coefficient** | $K_i$ | $0.0021\text{ A/K}$ | Short-circuit current sensitivity |
| **Diode Ideality Factor** | $a$ | $1.3$ | Recombination mechanism parameter |

### Characteristic Curves:

[images/curves_model.png]

Below are the resulting curves verifying exact alignment with theoretical limits under $1000\text{ W/m}^2$ irradiance:

| P-V Power Curve | I-V Current Curve |
| :---: | :---: |
| [images/P-V%20Curve.png] | [images/I-V%20Curve.png] |

---

## 2️⃣ Primary Control Strategy: Perturb & Observe (P&O)

The primary MPPT algorithm selected and deployed for our system is **Perturb and Observe (P&O)**. The controller introduces periodic perturbations to the operating voltage/duty cycle ($\Delta D$) while continuously calculating power changes ($\Delta P = P_k - P_{k-1}$) and voltage changes ($\Delta V = V_k - V_{k-1}$).

### Algorithmic Flowchart & Firmware Implementation:

The logic tree dictates whether to step the duty cycle up or down to navigate toward the peak:

| Decision Flowchart (`po_flowchart.png`) | Simulink Block Implementation (`MPPT_PO1.slx`) |
| :---: | :---: |
| [images/po_flowchart.png] | [images/mppt_logic.png] |

* **Embedded C Implementation (`mppt_po.c`):** Implements the state machine in fixed/floating-point C, handling sensor analog-to-digital conversions, threshold deadbands to suppress steady-state oscillation, and duty cycle saturation limits ($D_{min} = 0.02, D_{max} = 0.92$).

### Baseline Closed-Loop Testbench (`Incremental_conductance.slx`):

Used during early design phases to evaluate dynamic tracking and compare convergence speed:
[images/full_system.png]

| Tracking Performance Scope | Voltage, Current & Power Waveforms |
| :---: | :---: |
| [images/scope.png] | [images/scope1.png] |

---

## 3️⃣ Advanced Global Scanning MPPT for Partial Shading (`Central_Boost_MPPT_with_check.slx`)

Standard hill-climbing algorithms (like classical P&O) inherently fail under Partial Shading Conditions (PSC), frequently trapping the converter at a local power peak (LMPP).

### Forced Voltage Sweep Mechanism:

To guarantee absolute maximum power extraction, we developed a custom **Global Scanning** supervisory layer:

1. **Periodic Trigger:** Periodically commands an automated voltage sweep across the PV array terminal voltage.
2. **Sweep Window:** Sweeps operating voltage from estimated open-circuit potential down to minimum operating voltage threshold ($V_{scan\_min} = 240\text{ V}$).
3. **Peak Memorization:** Continuously logs power values during the sweep and identifies the global peak coordinate ($V_{best}, P_{best}$).
4. **Target Lock:** Returns and locks the operating point directly at $V_{best}$, restoring standard fine-step tracking around the true global maximum.

| Central Array System Topology | Control Subsystems & Measurements |
| :---: | :---: |
| [images/Central_Boost_MPPT_with_check_Page1.png] | [images/Central_Boost_MPPT_with_check_Page2.png] |

### Global Search Dynamic Scopes:

The scopes below illustrate the forced scan mechanism sweeping across power curves, discovering the true global peak, and locking the operating point:

| Global Scan Power Tracking | V-I-P Scan Dynamics |
| :---: | :---: |
| [images/forced_mppt_logic2.png] | [images/forced_mppt_logic.png] |

---

## 🔧 How to Run

1. Open MATLAB and navigate to `1_PV_Modeling_and_MPPT/`.
2. For analytical I-V/P-V characteristics, run `pv_curves.slx`.
3. To inspect P&O tracking logic, review `MPPT_PO1.slx` alongside `mppt_po.c`.
4. For partial shading global tracking evaluation, run `Central_Boost_MPPT_with_check.slx`.

