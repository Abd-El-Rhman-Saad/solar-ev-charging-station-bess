// =========================================================================
// Thesis Listing 6.1: Embedded Arduino C++ Hybrid Global Sweep & P&O MPPT
// Reference: Graduation Project Thesis §6.1.5 (Listing 6.1)
// Source: graduation project team firmware
// Notes: MATLAB-style '%' comments from thesis converted to '//' for compilation.
// NOTE: Added Serial.print("(") to close the parenthesis printed by the original listing.
// Registers: Mode 14 Fast PWM (ICR1 = 1600, 10 kHz).
// =========================================================================

// Embedded MPPT Core Control Firmware -- Interleaved Boost Converter Layout
const int voltagePin = A0;
const int currentPin = A1;

// Sensor calibration constants
const float V_CONVERSION = 0.0654;
const float ACS_OFFSET = 514.3;
const float ACS_SENS = 0.2121;

// Hardware Timer register parameters
const unsigned int TOTAL_TICKS = 1600; // Defines PWM period base frequency
unsigned int currentDutyTicks = 320;
unsigned int bestDutyTicks = 320;
float maxPowerFound = 0.0;
bool isSweeping = true;

unsigned long prevMillis = 0;
const int sweepStep = 16;
const int sweepDelay = 150;

float prevPower = 0.0;
int perturbDirection = 1;
const int perturbStep = 4; // Step resolution for closed-loop fine tracking

void setup() {
  pinMode(9, OUTPUT); // Phase 1 Gate Output
  pinMode(10, OUTPUT); // Phase 2 Gate Output

  // Reset Timer 1 Control Registers
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;

  // Configure Phase and Frequency Correct PWM Mode (Mode 9) / Fast PWM (Mode 14)
  TCCR1A |= (1 << WGM11);
  TCCR1B |= (1 << WGM13) | (1 << WGM12);

  // Enable non-inverting PWM output on Channel A (Pin 9)
  TCCR1A |= (1 << COM1A1);
  TCCR1A &= ~((1 << COM1B1) | (1 << COM1B0));

  // Set top boundary and initial duty cycle tracking variables
  ICR1 = TOTAL_TICKS;
  OCR1A = currentDutyTicks;
  OCR1B = TOTAL_TICKS / 2; // Symmetrical trigger for phase interleave ISR

  // Activate Timer 1 Compare Match B interrupt
  TIMSK1 |= (1 << OCIE1B);
  TCCR1B |= (1 << CS10); // No prescaling for optimal high-frequency switching

  Serial.begin(115200);
  Serial.println("=================================================");
  Serial.println(" MPPT Interleaved Boost -- Starting Sweep");
  Serial.println("=================================================");
  Serial.println("Duty\tTicks\tV_pv(V)\t\tI_pv(A)\t\tP_pv(W)");
}

// Interrupt Service Routine for interleaved 180-degree phase shift execution
ISR(TIMER1_COMPB_vect) {
  PORTB |= (1 << PORTB2); // Fire high-frequency trigger flag

  unsigned int currentDuty = OCR1A;

  // Hardware tracking loop synchronization loop
  while (TCNT1 < (TOTAL_TICKS / 2) + currentDuty) {
    if (TCNT1 < (TOTAL_TICKS / 2) && TCNT1 >= (currentDuty - (TOTAL_TICKS / 2))) {
      break;
    }
  }

  PORTB &= ~(1 << PORTB2); // Clear trigger flag
}

// Oversampled digital filter function for analog signal noise attenuation
float readAveragedADC(int pin, int samples = 64) {
  long sum = 0;
  for (int i = 0; i < samples; i++) {
    sum += analogRead(pin);
    delayMicroseconds(50);
  }
  return sum / (float)samples;
}

void loop() {
  if (isSweeping) {
    // --- STAGE 1: GLOBAL SWEEP TRACE OVER THE PV BOUNDARIES ---
    if (millis() - prevMillis >= sweepDelay) {
      prevMillis = millis();

      float pvVoltage = readAveragedADC(voltagePin) * V_CONVERSION;
      float rawCurrent = readAveragedADC(currentPin);
      float Iactual = (rawCurrent - ACS_OFFSET) * (5.0 / 1023.0) / ACS_SENS;
      if (Iactual < 0) Iactual = 0;

      float pvPower = pvVoltage * Iactual;
      float activeDuty = (float)currentDutyTicks / TOTAL_TICKS;

      Serial.print(activeDuty, 2); Serial.print("\t");
      Serial.print(currentDutyTicks); Serial.print("\t");
      Serial.print(pvVoltage, 2); Serial.print("\t\t");
      Serial.print(Iactual, 3); Serial.print("\t\t");
      Serial.println(pvPower, 2);

      // Record maximum power envelope
      if (pvPower > maxPowerFound) {
        maxPowerFound = pvPower;
        bestDutyTicks = currentDutyTicks;
      }

      currentDutyTicks += sweepStep;

      // Evaluate sweep completion status
      if (currentDutyTicks > 1440) {
        isSweeping = false;
        currentDutyTicks = bestDutyTicks;
        OCR1A = currentDutyTicks; // Lock hardware to peak point duty
        prevPower = maxPowerFound;

        Serial.println("-------------------------------------------------");
        Serial.print("SWEEP DONE! Peak Power : ");
        Serial.print(maxPowerFound, 2);
        Serial.println(" W");
        Serial.print("Locking at ticks : "); Serial.print(bestDutyTicks);
        Serial.print(" (");
        Serial.print(((float)bestDutyTicks / TOTAL_TICKS) * 100.0, 1);
        Serial.println("% Duty)");
        Serial.println("-------------------------------------------------");
        Serial.println("Entering P&O fine-tracking...");
        Serial.println("Dir\tTicks\tV_pv(V)\t\tI_pv(A)\t\tP_pv(W)");
      } else {
        OCR1A = currentDutyTicks;
      }
    }
  } else {
    // --- STAGE 2: CLOSED-LOOP PERTURB & OBSERVE FINE REGULATION ---
    if (millis() - prevMillis >= 1500) {
      prevMillis = millis();

      float pvVoltage = readAveragedADC(voltagePin) * V_CONVERSION;
      float rawCurrent = readAveragedADC(currentPin);
      float Iactual = (rawCurrent - ACS_OFFSET) * (5.0 / 1023.0) / ACS_SENS;
      if (Iactual < 0) Iactual = 0;

      float pvPower = pvVoltage * Iactual;

      // Evaluate gradient change to adapt perturbation direction
      if (pvPower < prevPower) {
        perturbDirection = -perturbDirection;
      }

      currentDutyTicks += perturbDirection * perturbStep;

      // Enforce hardware safe operating boundary limits
      if (currentDutyTicks > 1550) currentDutyTicks = 1550;
      if (currentDutyTicks < 160) currentDutyTicks = 160;

      OCR1A = currentDutyTicks; // Re-update hardware PWM register
      prevPower = pvPower;

      Serial.print(perturbDirection > 0 ? " UP\t" : "DOWN\t");
      Serial.print(currentDutyTicks); Serial.print("\t");
      Serial.print(pvVoltage, 2); Serial.print("\t\t");
      Serial.print(Iactual, 3); Serial.print("\t\t");
      Serial.println(pvPower, 2);

      // Monitor optimal current convergence at Maximum Power Point
      if (Iactual >= 0.85) {
        Serial.print(" *** Near MPP --- I_pv = ");
        Serial.print(Iactual, 3);
        Serial.println(" A ***");
      }
    }
  }
}
