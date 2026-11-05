// =========================================================================
// Thesis Listing 6.2: Open-Loop Diagnostic Firmware (10kHz Timer 1 PWM)
// Reference: Graduation Project Thesis §6.2.5.1 (Listing 6.2)
// Source: graduation project team firmware
// Notes: Sets 50% duty cycle at 10 kHz on Timer 1 (Mode 10 Phase-Correct PWM, ICR1 = 800)
// NOTE: The thesis comment describes this as 'Fast PWM', but the register setup (WGM13 + WGM11) operates in Mode 10 (Phase-Correct PWM, 10 kHz).
// =========================================================================

/**
 * 50% Duty Cycle at 10kHz using Timer1
 * Pin 9 is the designated output
 */

const int PWM_PIN = 9;
const int TOP_PWM = 800; // Value that determines the 10kHz frequency

void setup() {
  pinMode(PWM_PIN, OUTPUT);

  // Stop timer for configuration and clear registers
  TCCR1A = 0;
  TCCR1B = 0;

  // Configure Phase-Correct PWM mode using ICR1 as the TOP value
  // COM1A1: Set pin to Clear OC1A on Compare Match
  // WGM11 & WGM13: Set timer mode (Mode 10)
  TCCR1A = _BV(COM1A1) | _BV(WGM11);
  TCCR1B = _BV(WGM13) | _BV(CS10);

  ICR1 = TOP_PWM;

  // Set 50% Duty Cycle (50% of 800 = 400)
  OCR1A = 400;
}

void loop() {
  // No code needed in the main loop;
  // The timer executes autonomously in the background via hardware timer
}
