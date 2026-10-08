# ☀️ Solar-Powered EV Charging Station with Battery Energy Storage (BESS)

![MATLAB/Simulink](https://img.shields.io/badge/MATLAB-Simulink-blue?logo=mathworks)
![Altium Designer](https://img.shields.io/badge/Altium-Designer-orange)
![Control Systems](https://img.shields.io/badge/Control-Closed_Loop-success)
![Power Electronics](https://img.shields.io/badge/Domain-Power_Electronics-red)

> **B.Sc. Graduation Project 2026** — Electrical Power and Machines Department, Faculty of Engineering, Alexandria University.
> Thesis title: *Integration of Photovoltaic Systems and Energy Storage for Electric Vehicle Charging Station*.

## 📌 Executive Summary
This repository contains the design, simulation models, and PCB design files for a **standalone solar-powered Electric Vehicle (EV) charging station**. A photovoltaic (PV) array feeds a **600 V DC link**; a **Battery Energy Storage System (BESS)** balances the difference between solar generation and EV demand, so charging does not depend on the grid.

The project connects three layers of work: **mathematical modeling** (state-space averaging, PI design), **simulation** (MATLAB/Simulink, including a Global MPPT algorithm for partial shading), and a **hardware prototype** (Altium-designed converter boards driven by an Arduino).

---

## 🏗️ System Architecture & Power Flow

Everything is connected through a stabilized **600 V DC link**:

1. **Generation:** PV array.
2. **Step-up stage:** an **Interleaved Boost Converter** with MPPT raises the PV voltage to the DC link.
3. **Storage:** the **BESS** is connected through a **Two-Quadrant (bidirectional) Chopper** that charges or discharges the battery to hold the DC-link voltage.
4. **Load:** a **Buck Converter** steps the DC link down to the EV battery voltage.

### 📊 Full-system simulation snapshot (actual rating)
| Item | Value |
| :--- | :--- |
| DC-link voltage | 600 V |
| PV power at the studied operating point | ≈ 8.52 kW |
| EV load | 12 kW |
| Power supplied by the BESS to cover the deficit | ≈ 3.48 kW |

### 🔧 Hardware prototype (see thesis, Chapter 6)
| Item | Value |
| :--- | :--- |
| Switching frequency | 10 kHz |
| Power switches | IRFP260N MOSFETs |
| Gate drive | 6N139 optocoupler + IR2111 half-bridge driver (two-quadrant chopper board) |
| Inductor | 2 mH |
| Controller | Arduino Uno (fixed-point PI firmware) |
| Current tracking | within ≤ 5% of the reference (0.2–0.4 A range) |
| Battery test | CC charging of a 2S Li-ion pack verified with oscilloscope/DMM |

---

## 🚀 Modules

### 1. PV Array & Global MPPT (`1_PV_Modeling_and_MPPT/`)
Standard MPPT algorithms can lock onto a **local** maximum under **Partial Shading Conditions (PSC)**.
* **Models:** a parameterized single-diode PV model, a Perturb & Observe block, an Incremental Conductance system, and a **Global MPPT (forced voltage sweep)** system.
* **Global MPPT:** a four-state machine (reset → sweep V from 300 V down to 240 V → return to the best voltage → hold for 1 s, then re-scan) that finds the global maximum power point.

### 2. Interleaved Boost Converter (`2_Interleaved_Boost_Converter/`)
* Two boost phases operating 180° apart reduce the **input-current ripple** and the stress on the input capacitor.
* Development path: open-loop single-phase → open-loop interleaved → Thevenin-equivalent source for PI tuning → full closed-loop interleaved model.

### 3. BESS & Two-Quadrant Chopper (`3_BESS_and_2Q_Chopper/`)
* A bidirectional DC-DC converter that lets the battery absorb or supply power.
* **Dual-loop control:** an outer voltage PI loop regulates the DC link; an inner current loop (hysteresis control in the Simulink models, fixed-point PI on the Arduino in the hardware) limits battery current to **+3.1 A (charging) / −10 A (discharging)**.
* **Bidirectional CC-CV supervisory logic:** documented in the thesis (Sec. 5.6.2). It keeps the battery inside safe limits by switching to voltage clamping when the pack is nearly full or empty.

### 4. EV Buck Charger (`4_EV_Buck_Charger/`)
* **Open-loop model (`Buck.slx`):** validates component sizing, switching and ripple of the power stage.
* **Closed-loop CC-CV charging:** the PI current loop (CC stage) and PI voltage loop (CV stage) are designed and simulated in the thesis (Chapter 5, Sec. 5.2.3). The CC stage was validated on hardware (Chapter 6). The closed-loop `.slx` models are not included in this repository yet.

### 5. Hardware & PCB Design (`5_Hardware_and_PCB_Design/`)
* Altium Designer projects for the three converter boards (Interleaved Boost, Two-Quadrant Chopper, Buck).
* Layout practices: wide polygon pours for high-current nodes, clearance/creepage rules for the high-voltage side, and star grounding to keep switching noise away from the microcontroller.

---

## 📘 Documentation

* **Full thesis (PDF, 287 pages):** [`Docs/Final Graduation Project Documentation - 2026.pdf`](Docs/Final%20Graduation%20Project%20Documentation%20-%202026.pdf) — state-space derivations, transfer functions, component sizing, control design, simulation results and hardware validation.
* **LaTeX source of the thesis:** [`Docs/LaTeX_Source/`](Docs/LaTeX_Source).
* **Final defense presentation:** available in the [**GitHub Releases**](../../releases/latest) section.

---

## 💻 How to Use This Repository

1. **Requirements:** MATLAB/Simulink with Simscape Electrical for the `.slx` files; Altium Designer for the PCB projects (extract the `.zip` archives first).
2. **Navigation:** every folder has its own `README.md` describing its files.
3. **Running a model:** open the `.slx` file and press **Run**. If a model needs workspace variables, run the accompanying initialization script first.

---

## 👥 Team & Supervision

Developed by a team of nine students:
* Abd El-Rhman Muhammad Saad
* Nour El-Deen Mohamed Fathy
* Yasmeen Salah Khobeez
* Mariam Mamdouh Ibrahim
* Mai Nagah Ali
* Rwan Mohamed Abd-Elmokhtar
* Rewan Amr Mahmoud
* Youssef Ebrahim Abd El-Sattar

**Supervisor:** Prof. Dr. Ahmed Abbas El Serougi.

**Contribution of the repository owner (Abd El-Rhman Muhammad Saad):** main role on the **Two-Quadrant Chopper** (design, firmware and testing); the team worked together on all boards and on the thesis.
