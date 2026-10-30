# 🔋 Battery Energy Storage System (BESS) & Two-Quadrant Chopper

This directory contains the analytical derivations, digital control design, and MATLAB/Simulink simulations for the **Two-Quadrant (2Q) Bidirectional DC-DC Chopper**. Within our Solar-Powered EV Charging Station, this power electronics interface acts as the central energy buffer between the 600V DC-Link and the Battery Energy Storage System (BESS).

The models document our full engineering lifecycle: from low-power open-loop hardware prototyping to full-scale closed-loop cascaded control, including discrete-time Z-domain transformation for embedded microcontroller deployment.

---

## ⚡ Bidirectional Operational Regimes & Power Flow

The Two-Quadrant Chopper controls bidirectional power flow through a split-leg configuration comprising two active switches ($Q_1$ High-Side, $Q_2$ Low-Side) and an interfacing power inductor ($L = 2\text{ mH}$):

```text
[ 600V DC BUS ]
              │
         ┌────┴────┐
         │ Q1 (HS) │  (Buck Switch / Boost Diode)
         └────┬────┘
              ├───────────[ Inductor L ]───► [ BESS Battery Pack ]
         ┌────┴────┐
         │ Q2 (LS) │  (Boost Switch / Buck Diode)
         └────┬────┘
              │
          [ GROUND ]
```

### 1️⃣ Buck Mode (Battery Charging — Solar Surplus)

* **Trigger:** When solar PV generation exceeds EV load demand ($P_{PV} > P_{EV}$).
* **Operation:** The converter acts as a step-down regulator. The high-side switch $Q_1$ is driven by PWM duty cycle $D_{ch}$, while the anti-parallel diode of $Q_2$ provides the freewheeling path.
* **Governing Equation:**
  $$
  V_{batt} = D_{ch} \cdot V_{DC}
  $$
* **Current Convention:** Battery current is negative (entering the battery), clamped at our maximum safe continuous charge limit ($-3.1\text{ A}$ on prototype).

### 2️⃣ Boost Mode (Battery Discharging — Solar Deficit)

* **Trigger:** When EV charging demand exceeds solar output ($P_{PV} < P_{EV}$), or during zero-solar night intervals.
* **Operation:** The converter operates as a step-up regulator. The low-side switch $Q_2$ is modulated with duty cycle $D_{dis}$, while $Q_1$ freewheels, stepping up battery potential to hold the DC bus at 600V.
* **Governing Equation:**
  $$
  V_{DC} = \frac{V_{batt}}{1 - D_{dis}}
  $$
* **Current Convention:** Battery current is positive (discharging into the bus), providing up to $3.48\text{ kW}$ of deficit support ($+10\text{ A}$ limit on prototype).

### 3️⃣ Shoot-Through Protection:

A mandatory hardware dead-time ($t_{dead} \approx 1\ \mu\text{s}$) is embedded between complementary gate pulses to prevent simultaneous conduction across the high-voltage DC-link.

---

## 🧠 Digital Control Architecture & Z-Domain Discretization

The system utilizes a cascaded closed-loop control structure: an outer Voltage Loop (holding the 600V bus) and an inner Current Loop (tracking battery current with rapid dynamic response).

### 1. Continuous S-Domain Formulation:

The baseline PI controller is formulated in the continuous domain as:

$$
C(s) = K_p + \frac{K_i}{s}
$$

### 2. Bilinear (Tustin) Transformation:

To deploy this controller on real hardware (Arduino Uno / ESP32) without numerical instability, the transfer function was mapped to the discrete $z$-domain using the Tustin approximation:

$$
s \approx \frac{2}{T_s} \frac{1 - z^{-1}}{1 + z^{-1}}
$$

Substituting into $C(s)$ yields the discrete-time transfer function:

$$
C(z) = \frac{U(z)}{E(z)} = \frac{(2K_p + K_i T_s) + (K_i T_s - 2K_p)z^{-1}}{2(1 - z^{-1})}
$$

### 3. Difference Equation for Embedded Fixed-Point C++:

Expanding into the time-domain execution loop:

$$
u[k] = u[k-1] + a_0 \cdot e[k] + a_1 \cdot e[k-1]
$$

Where:

$$
a_0 = K_p + \frac{K_i T_s}{2}, \quad a_1 = -K_p + \frac{K_i T_s}{2}
$$

*(This difference equation was implemented using fixed-point integer scaling on the microcontroller to avoid slow floating-point execution, ensuring a control execution loop well within the $10\text{ kHz}$ switching period).*

---

## 📊 Full-Scale System vs. Laboratory Prototype Sizing

| Engineering Parameter | Full-Scale Utility BESS | Scaled Laboratory Prototype |
| :--- | :---: | :---: |
| **Common DC Bus Potential** | $600.0\text{ V}$ | $12.0\text{ V} - 24.0\text{ V}$ |
| **Target Power Capacity** | Covers **$3.48\text{ kW}$** deficit | Validates safe 2S Li-ion pack dynamics |
| **Battery Chemistry** | Lithium-ion Battery Bank | 2S Li-ion pack ($7.4\text{ V}$ nom. / $8.4\text{ V}$ max) |
| **Power Semiconductor** | High-Power IGBT / SiC Modules | IRFP260N Power MOSFETs (200V, 50A) |
| **Gate Driver Isolation** | High-Voltage Optocoupled Drivers | 6N139 / 6N137 Optocoupled IR2111 |
| **Switching Frequency** | $10\text{ kHz}$ | $10\text{ kHz}$ |
| **Inductor Sizing** | Power filter choke | $2.0\text{ mH}$ Iron-Core Inductor |
| **Current Limits** | Scaled pack bounds | $-3.1\text{ A}$ (Charge) / $+10.0\text{ A}$ (Discharge) |
| **Current Tracking Error** | $< 2\%$ | $\le 5\%$ (within $0.2\text{ A} - 0.4\text{ A}$) |

---

## 📂 File Architecture & Verification Progression

The models capture our complete progressive verification workflow:

### Phase 1: Open-Loop Baseline Testing

* **`chopper_prototype_rating.slx`**: Open-loop testbench configured at low-power ratings. Validates gate switching logic, duty cycle sensitivity, and component thermal margins on our laboratory bench.
* **`chopper_actual_rating.slx`**: Open-loop model scaled up to high-voltage EV battery ratings, verifying converter passive sizing prior to closing feedback loops.

### Phase 2: Closed-Loop Control & Dynamic Validation

* **`Quadrant2chopper2.slx`**: Initial closed-loop testbench tuning the PI coefficients and validating isolated charging or discharging steps.
* **`final2q_all_cases.slx`**: The comprehensive, production-grade closed-loop model. Simulates dynamic transitions between charging (buck mode) and discharging (boost mode), enforcing current saturation limits and verifying DC bus stability.

---

## 🏗️ System Architecture & Code Implementation

**Two-Quadrant Chopper Closed-Loop Model (`final2q_all_cases.slx`):**

![2Q Chopper Model](images/2q_chopper_model.png)

**Embedded Control State Machine & Limits:**
The MATLAB Function block below illustrates the inner current tracking and strict saturation bounds ($-3.1\text{ A}$ charging, $+10\text{ A}$ discharging):

```matlab
function [Gate_High, Gate_Low] = fcn(Vdc_meas, Ibatt_meas, Vdc_ref)
    % --- PART 1: Memory (Persistent Variables) ---
    persistent int_err_v S_high
    if isempty(int_err_v)
        int_err_v = 0;
        S_high = 0;
    end

    % --- Internal Constants ---
    Ts = 5e-5;      % Sampling time (from your powergui)
    h = 0.05;        % Hysteresis band
    Kp_v = 0.1;     % PI Proportional Gain
    Ki_v = 10;      % PI Integral Gain

    % --- PART 2: Outer Loop (Voltage PI Control) ---
    err_v = Vdc_meas - Vdc_ref ;
    int_err_v = int_err_v + err_v * Ts;
    
    % Anti-windup (Clamping the integrator)
    int_err_v = max(min(int_err_v, 5), -5); 
    
    % Calculate the Current Reference (I_ref)
    I_ref = (Kp_v * err_v) + (Ki_v * int_err_v);

    % --- PART 3: Current Saturation (Protection) ---
    % Logic: Charging = Positive (+3.1A max), Discharging = Negative (-10A max)
    I_max_charge = 3.1;   
    I_max_disch  = -10.0; 
    
    % This line is Part 3: It clamps the reference to your hardware limits
    I_ref_sat = max(I_max_disch, min(I_max_charge, I_ref));

    % --- PART 4: Inner Loop (Hysteresis Current Control) ---
    % S_high = 1: Top IGBT ON (Increases current)
    % S_high = 0: Bottom IGBT ON (Decreases current)
    if Ibatt_meas < (I_ref_sat - h)
        S_high = 1;
    elseif Ibatt_meas > (I_ref_sat + h)
        S_high = 0;
    end

    % --- PART 5: Gate Outputs ---
    Gate_High = S_high;
    Gate_Low  = 1 - S_high; % Complementary switching
end
```
---

## 📈 Experimental Simulation Results

The scope capture below verifies seamless bidirectional transitions. As power balance shifts, the battery current dynamically reverses direction across the zero-axis while holding the DC-Link voltage tightly regulated:


![Bidirectional Scope](images/bidirectional_scope.png)

---

## 🔧 How to Run

1. Open MATLAB and navigate to `3_BESS_and_2Q_Chopper/`.
2. Open `final2q_all_cases.slx` to inspect full closed-loop bidirectional operation.
3. Run the simulation and observe the Scope blocks to verify current reversal dynamics and DC bus regulation during charging and discharging transients.
   

