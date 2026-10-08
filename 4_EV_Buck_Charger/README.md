# 🚙 EV Buck Charger

Simulation of the final power stage: a buck converter that steps the **600 V DC link** down to the voltage of the EV battery.

## 📂 Files

| File | Status |
| :--- | :--- |
| `Buck.slx` | Open-loop model of the buck power stage (component sizing, switching, ripple). Included. |
| Closed-loop CC-CV models | **Not included in this repository yet.** The design and simulation are documented in the thesis (Chapter 5, Sec. 5.2.3). |

---

## ⚙️ Control Strategy: CC-CV Charging
Battery charging follows a **Constant Current – Constant Voltage (CC-CV)** profile:

1. **Constant Current (CC):** when the battery is empty or at low state of charge, a PI loop regulates the charging current to a safe maximum while the battery voltage rises.
2. **Constant Voltage (CV):** when the battery approaches full charge, a PI voltage loop clamps the output voltage and the current decays naturally.

The CC stage was also **validated on the hardware prototype** (thesis, Chapter 6).

---

## 📊 Open-Loop Results
The open-loop model checks that the converter steps the DC-link voltage down and delivers stable power to a resistive load.

**Model:**
![Open-Loop Buck](images/Buck.png)

**Output voltage and current** (the zoomed view checks that the switching ripple stays within the design target, e.g. inductor ripple ΔI<sub>L</sub> ≤ 20%):

![V-I Output](images/VI.png)
![V-I Ripple Details](images/VI_zoomed.png)

**Output power:**
![Power Output](images/power.png)
![Power Ripple Details](images/power_zoomed.png)

---

### 🔧 How to Run
1. Open MATLAB and navigate to this folder.
2. Open `Buck.slx`.
3. Press **Run** and inspect the output ripple and steady-state values in the Scope blocks.
