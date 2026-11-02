# 🚗 EV Buck Charger: Power Stage, Hardware Prototyping & Closed-Loop CC/CV Control

This directory contains the analytical derivations, hardware parasitic calculations, discrete control design, and MATLAB/Simulink simulations for the **DC-DC Step-Down (Buck) Converter** stage. Within our Solar-Powered EV Charging Station, this power electronics interface steps down the regulated 600 V DC-Link bus to safely and efficiently charge the Electric Vehicle (EV) battery pack under an automated Constant-Current / Constant-Voltage (CC/CV) protocol.

The models document our complete engineering lifecycle: from high-power open-loop continuous-conduction sizing ($50\text{ kW}$) to laboratory hardware prototyping under non-ideal parasitic conduction losses, culminating in a fully closed-loop multi-stage CC/CV battery charger with automated supervisory transition logic.

---

## 📌 File Architecture & Simulation Manifest

| Simulation Model | Operational Mode | Power & Voltage Ratings | Primary Engineering Objective |
| :--- | :---: | :---: | :--- |
| **`Buck_Power_Stage_Open_Loop.slx`** | Open-Loop | $V_{\text{in}} = 600\text{ V} \to V_o = 400\text{ V}$ ($50\text{ kW}$) | High-power converter stage sizing, continuous conduction mode (CCM), and ripple limits. |
| **`Buck_Prototype_Current_Control.slx`** | Closed-Loop | $V_{\text{in}} = 10\text{ V} \to I_{\text{ref}} = 0.74\text{ A}$ ($10\text{ kHz}$) | Prototype-scale current regulation using a discrete PI controller and sawtooth PWM generator. |
| **`Buck_Closed_Loop_CCCV_Charger.slx`** | Closed-Loop | $V_{\text{in}} = 600\text{ V} \to V_{\text{batt}} = 380\text{ V} / 420\text{ V}$ | Full multi-stage battery charging model with automated CC-to-CV transition logic. |

---

## 📐 Non-Ideal Circuit Modeling & Parasitic Compensation

In practical power electronics, theoretical duty cycles derived from ideal switches fail to achieve target output voltages due to internal semiconductor drops and parasitic line resistances. As derived in Section 6.3 of the project thesis, the hardware prototype incorporates measured non-idealities:

* Inductor DC winding resistance: $r_L = 1.9\ \Omega$
* Output capacitor Equivalent Series Resistance (ESR): $r_C = 0.2\ \Omega$
* Power MOSFET on-state channel resistance: $r_{\text{DS(on)}} = 0.025\ \Omega$
* Freewheeling diode forward voltage drop: $V_{\text{fwd}} = 0.8\text{ V}$
* Switching frequency: $f_{\text{sw}} = 10\text{ kHz}$

### Non-Ideal Duty Cycle Derivation ($D_u$)

Applying volt-second balance across the inductor over a complete switching period ($T_{\text{sw}}$) yields:

$$D_u = \frac{V_{\text{fwd}} + I_L \cdot r_L + V_o}{V_{\text{in}} - I_L \cdot r_{\text{DS(on)}} + V_{\text{fwd}}}$$

For our hardware operating point ($V_{\text{in}} = 12.3\text{ V}$, $V_o = 3.7\text{ V}$, $R_{\text{load}} = 10\ \Omega \implies I_L = 0.37\text{ A}$):

$$D_u = \frac{0.8 + (0.37 \cdot 1.9) + 3.7}{12.3 - (0.37 \cdot 0.025) + 0.8} = \frac{5.203}{13.09075} \approx \mathbf{39.75\%}$$

*(Compared to an ideal duty cycle of $D_{\text{ideal}} = \frac{3.7}{12.3} \approx 30.08\%$, the additional $9.67\%$ duty cycle directly compensates for internal semiconductor conduction losses).*

### Minimum Component Sizing & Hardware Margin

Allowing an inductor current ripple target $\Delta I_L = 0.222\text{ A}$ ($60\%$ of average current) and an output voltage ripple target $\Delta V_o = 0.074\text{ V}$ ($2\%$ ripple tolerance):

$$L_{\text{min}} = \frac{\left[V_{\text{in}} - I_L(r_{\text{DS(on)}} + r_L) - V_o\right] \cdot D_u}{\Delta I_L \cdot f_{\text{sw}}} \approx 1.412\text{ mH} \implies \text{Selected Hardware: } \mathbf{2.0\text{ mH}}$$

$$C_{\text{min}} = \frac{\Delta I_L}{8 \cdot \Delta V_o \cdot f_{\text{sw}}} = \frac{0.222}{8 \cdot 0.074 \cdot 10000} \approx 37.5\ \mu\text{F} \implies \text{Selected Hardware: } \mathbf{200\ \mu\text{F}}$$

---

## 🔋 Closed-Loop CC/CV Battery Charging Control

Lithium-ion battery charging requires two distinct operational regimes to maximize energy intake while preventing cell overheating and chemical degradation:

| Charging Regime | Operational Condition | Control Objective | Terminal Dynamics |
| :--- | :--- | :--- | :--- |
| **Constant Current (CC)** | $V_{\text{batt}} < 420.0\text{ V}$ | Inner current PI loop regulates charging current at rated limit ($I_{\text{rated}} = -50\text{ A}$). | Terminal voltage rises steadily as bulk capacity is replenished. |
| **Constant Voltage (CV)** | $V_{\text{batt}} \ge 420.0\text{ V}$ | Outer voltage PI loop clamps terminal voltage strictly at $420.0\text{ V}$. | Charging current decays exponentially; cutoff occurs as battery reaches full capacity. |

### CC-to-CV Supervisory Transition Logic

The transition is governed by a high-speed comparator monitoring battery terminal potential:
* While $V_{\text{batt}} < 420\text{ V}$, Switch 1 is closed, routing the inner current loop error to generate the PWM gating pulses.
* As soon as $V_{\text{batt}}$ touches $420.0\text{ V}$ (at $t = 6.2\text{ s}$), Switch 1 opens and Switch 2 closes, transferring authority to the voltage clamp PI loop.
* This bumpless transfer eliminates inductive current spikes and guarantees a smooth exponential taper down to float charge.

---

## 📐 Architecture Schematics & Waveform Verification

### 1. High-Power Open-Loop Power Stage (50 kW)
Evaluates continuous conduction step-down capability from the $600\text{ V}$ DC-Link bus delivering $50\text{ kW}$ to a high-power load ($V_{\text{in}} = 600\text{ V}$, $V_o = 400\text{ V}$, $L = 1\text{ mH}$, $C = 470\ \mu\text{F}$):

<p align="center">
  <img src="images/buck_open_loop_power_stage_300dpi.png" width="90%" alt="High-Power Open-Loop Buck Power Stage" />
</p>

### 2. Prototype-Scale Closed-Loop Current Control Model
Low-voltage hardware replica featuring discrete PI current regulation ($I_{\text{ref}} = 0.74\text{ A}$) and sawtooth pulse-width modulation at $10\text{ kHz}$:

<p align="center">
  <img src="images/buck_prototype_current_control_300dpi.png" width="90%" alt="Prototype-Scale Closed-Loop Buck Model" />
</p>

### 3. Prototype PI Current Regulation Loop
Subsystem architecture showing error computation, discrete PI tuning, and carrier wave comparison:

<p align="center">
  <img src="images/buck_prototype_control_loop_300dpi.png" width="75%" alt="Prototype PI Current Control Loop" />
</p>

### 4. Full Multi-Stage Closed-Loop CC/CV Charger Architecture
The complete high-power vehicle charger integrating battery dynamic models, CC current loop, CV voltage clamp, and supervisory transfer logic:

<p align="center">
  <img src="images/buck_cccv_full_system_300dpi.png" width="95%" alt="Full Multi-Stage Closed-Loop CC/CV Charger Architecture" />
</p>

### 5. Dual-Loop CC/CV Control Subsystem
Shows the parallel arrangement of the voltage and current PI regulators with automated logic switching:

<p align="center">
  <img src="images/buck_cccv_control_loop_300dpi.png" width="75%" alt="Dual-Loop CC/CV Control Subsystem" />
</p>

### 6. Complete Dynamic CC/CV Charging Cycle (Annotated Overview)
The full charging profile across time ($0\text{ to }15\text{ s}$), illustrating the linear voltage ramp during Constant Current mode, the transition at $t = 6.2\text{ s}$, and the exponential current decay during Constant Voltage mode:

<p align="center">
  <img src="images/Buck_CCCV_Scope.png" width="90%" alt="Complete Dynamic CC/CV Charging Cycle Waveform" />
</p>

### 7. High-Resolution 300 DPI Multi-Subplot Battery Dynamics
Crisp vector-grade scope output displaying $V_{\text{bat}}$ (Top), $I_{\text{bat}}$ (Middle), and State of Charge $\text{SOC}$ (Bottom):

<p align="center">
  <img src="images/buck_cccv_dynamic_curves_300dpi.png" width="85%" alt="300 DPI Multi-Subplot Battery Dynamics" />
</p>

### 8. Zoomed Waveform View in CV Mode
High-magnification view during Constant Voltage regulation ($t > 11.6\text{ s}$), verifying that voltage ripple is held under $\pm 0.05\text{ V}$ while current decays smoothly:

<p align="center">
  <img src="images/buck_cccv_zoomed_ripple_300dpi.png" width="85%" alt="Zoomed Waveform View in CV Mode" />
</p>

---

## 📂 Directory Structure & Simulation Execution Guide

```text
4_EV_Buck_Charger/
├── README.md                                          # Master technical documentation & parasitic sizing report
├── Buck_Power_Stage_Open_Loop.slx                     # High-power 50 kW open-loop buck power stage
├── Buck_Prototype_Current_Control.slx                 # Scaled prototype current regulation model
├── Buck_Closed_Loop_CCCV_Charger.slx                  # Full-scale CC/CV battery charger model
├── universal_export_scopes_300dpi_v5.m               # Multi-subplot 300 DPI automated scope exporter
├── universal_export_scopes_300dpi_v4.m               # Collision-free 300 DPI scope exporter
├── universal_export_system_diagrams_300dpi_v4.m      # Hierarchical 300 DPI system diagram exporter
└── images/                                            # 300 DPI high-resolution architecture & scope assets
    ├── Buck_CCCV_Scope.png
    ├── buck_cccv_control_loop_300dpi.png
    ├── buck_cccv_dynamic_curves_300dpi.png
    ├── buck_cccv_full_system_300dpi.png
    ├── buck_cccv_zoomed_ripple_300dpi.png
    ├── buck_open_loop_power_stage_300dpi.png
    ├── buck_prototype_control_loop_300dpi.png
    └── buck_prototype_current_control_300dpi.png
```

### Steps to Run the Charging Models

1. **Prerequisites:** Open MATLAB/Simulink R2022b or later with Simscape Electrical.
2. **Open-Loop Sizing Analysis:**
   * Open `Buck_Power_Stage_Open_Loop.slx` and run the simulation to inspect inductor current ripple and capacitor voltage filtering.
3. **Prototype Current Control Validation:**
   * Open `Buck_Prototype_Current_Control.slx`.
   * Observe how the discrete PI controller regulates charging current at exactly $0.74\text{ A}$ under $10\text{ kHz}$ switching.
4. **Full CC/CV Battery Charging Execution:**
   * Open `Buck_Closed_Loop_CCCV_Charger.slx`.
   * Run the simulation ($T_{\text{stop}} = 15.0\text{ s}$).
   * Open `Scope_0_Vbat_Ibat_SOC` to observe the full CC-to-CV transition at $t = 6.2\text{ s}$ as battery potential reaches $420.0\text{ V}$.
5. **Solver Recommendations:**
   * Solver: `Discrete (no continuous states)`.
   * Sample Time: $T_s = 20\ \mu\text{s}$ configured in the `powergui` block.
