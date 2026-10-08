# 🛠️ Hardware Prototyping & PCB Design

This folder holds the **Altium Designer** projects used to turn the simulated converters into hardware prototypes.

## 📂 Files

| Archive | Board |
| :--- | :--- |
| `Interleaved_Thevenin.zip` | Interleaved boost converter |
| `Two_Quadrant_Chopper.zip` | Two-quadrant (bidirectional) chopper |
| `Buck_Converter.zip` | EV buck converter |

Extract an archive and open the Altium project (`.PrjPcb`) to view the schematic (`.SchDoc`) and PCB layout (`.PcbDoc`). The archives also include the fabrication (CAM) output of the boards. The two-quadrant chopper archive additionally contains a PDF print of the schematic (`2Q_Final.pdf`).

## 🔌 Prototype Hardware
* **Power switches:** IRFP260N MOSFETs.
* **Two-quadrant chopper board:** the control signal from the microcontroller passes through a **6N139 optocoupler** and drives an **IR2111 half-bridge gate driver** (bootstrap supply) for the high-side and low-side MOSFETs.
* **Controller:** Arduino Uno running fixed-point PI firmware, switching at **10 kHz**.
* Voltage and current sensing circuits and the microcontroller interface are on the same boards.

## ⚡ Layout Practices
1. **High-current paths:** wide polygon pours instead of thin traces, to reduce resistance and avoid hot spots.
2. **High-voltage isolation:** clearance and creepage between the high-voltage DC side and the low-voltage control circuitry.
3. **Grounding:** separate power ground and signal ground joined at one point (star grounding), so switching noise does not disturb the analog measurements and the microcontroller.

---
*These boards are the physical prototypes used to validate the control strategies simulated in the other folders. Measured results are in the thesis (Chapter 6).*
