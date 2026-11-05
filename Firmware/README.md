# 💾 Firmware & Control Code

> **Team Project:** Solar-Powered EV Charging Station with BESS (8-member team). The control firmware was written primarily by teammate **Mai Nagah Ali**; the listings below are extracted from the graduation thesis LaTeX source (`Docs/LaTeX_Source/`). *Compiled and organised by Abd El-Rhman Muhammad Saad.*

The project controllers were implemented as MATLAB Functions in Simulink and as Arduino (ATmega328P, 16 MHz) firmware. Each folder below maps to a thesis section.

| Folder / File | Thesis reference | What it is |
| :--- | :--- | :--- |
| `MATLAB_Functions/Section_5.1.3.1_FSM_Global_MPPT_Iref.m` | §5.1.3.1 | FSM-based global MPPT, outputs `Iref` (voltage sweep 280 → 240 V, then P&O) |
| `MATLAB_Functions/Section_5.5.2_Central_Interleaved_Boost_MPPT_controller.m` | §5.5.2 | Central interleaved boost MPPT controller with PWM generation (`fsw = 10 kHz`, `IL1` feedback) |
| `Thesis_Listing_6.1_MPPT_Interleaved_Boost/` | §6.1.5, Listing 6.1 | Arduino hybrid global-sweep + P&O MPPT for the interleaved boost (serial logging, Timer 1 compare-B ISR) |
| `Thesis_Listing_6.2_OpenLoop_Diagnostic_10kHz/` | §6.2.5.1, Listing 6.2 | Open-loop 50 % duty, 10 kHz Timer 1 diagnostic (`ICR1 = 800`) |
| `Thesis_Listing_6.3_Buck_FixedPoint_PI_ISR/` | §6.2.5.2, Listing 6.3 | Fixed-point (`int32_t`, scaling 2^17) PI **current controller for the buck stage** (`BUCK_ICNTRL_PI`), executed in the Timer 1 overflow ISR (`ICR1 = 1599`, 10 kHz) |
| `Bench_Telemetry_Sketch_2Q_PI/` | not in the thesis | Floating-point PI loop that prints `I_meas \| Target \| OCR1A` to the Serial Monitor (log format of `5_Hardware_and_PCB_Design/images/serial_monitor.jpg`) |

## Notes on the extraction

* Code is verbatim from the thesis. Only two things changed: the MATLAB-style `%` comments in Listing 6.1 were converted to `//` so the sketch compiles, and a header comment was added to each file.
* **Thesis comment vs. registers:** Listing 6.2's comment says "Fast PWM", but the register setup (`WGM13 + WGM11`) is Phase-Correct PWM, Mode 10 (`f = 16 MHz / (2 × 800) = 10 kHz`). Listing 6.1's comment says "Mode 9", but the register setup (`WGM13 + WGM12 + WGM11`) is Fast PWM, Mode 14. The code was left unchanged.
* The telemetry sketch is *not* part of the thesis. It is kept as a separate, clearly labelled file because it matches the bench Serial Monitor log.
