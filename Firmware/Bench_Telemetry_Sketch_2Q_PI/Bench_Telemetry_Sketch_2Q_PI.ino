// =========================================================================
// Bench Telemetry Sketch: Floating-Point Current PI Loop with Serial Logging
// Source: graduation project team firmware (not part of the thesis listings; matches serial_monitor.jpg)
// Notes: Matches the Serial Monitor log format in 5_Hardware_and_PCB_Design/images/serial_monitor.jpg
// Loop Rate: ~200 Hz (delay(5)); Timer 1 Phase-Correct PWM (10 kHz, TOP = 800)
// =========================================================================

#include <Arduino.h>

// Hardware Pin Definitions
const int PWM_PIN    = 9;   // OC1A (Timer 1 Output A)
const int SENSOR_PIN = A0;  // ACS712 Current Sensor Output

// Target Current Setpoint
const float TARGET_I = 0.40; // Target Current (0.40 A)
const int   TOP_PWM  = 800;  // TOP Value for 10 kHz Phase-Correct PWM

// Calibration Constants for ACS712-05B
const float VREF         = 5.0;   // Measured ADC Reference Potential (Volts)
float       SENSITIVITY  = 0.185; // Hall Sensor Sensitivity (185 mV/A)
float       ZERO_VOLTAGE = 2.50;  // Quiescent Zero-Current Offset (Auto-Calibrated)

// Discrete PI Controller Gains
float Kp = 25.0;
float Ki = 1.2;

// Controller State Variables
float integral_err = 0.0;
int   ocr_value    = 400; // Initialize at 50% Duty Cycle

void setup() {
  Serial.begin(115200);

  // Configure OC1A (Pin 9) as digital output
  pinMode(PWM_PIN, OUTPUT);

  // Reset Timer 1 Control Registers
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1  = 0;

  // Configure Mode 10: Phase Correct PWM, TOP = ICR1
  // COM1A1 = 1, COM1A0 = 0 -> Clear OC1A on Compare Match up-counting, set down-counting
  TCCR1A |= (1 << COM1A1) | (1 << WGM11);
  TCCR1B |= (1 << WGM13)  | (1 << CS10); // Prescaler = 1

  ICR1  = TOP_PWM;  // Set TOP to 800 (10 kHz)
  OCR1A = ocr_value;

  // Auto-Zero Sensor Calibration over 100 samples
  long sum = 0;
  for (int i = 0; i < 100; i++) {
    sum += analogRead(SENSOR_PIN);
    delay(2);
  }
  ZERO_VOLTAGE = (sum / 100.0) * (VREF / 1023.0);
}

void loop() {
  // Read raw ADC current measurement (offset corrected by 100-sample auto-calibration at boot)
  int raw_adc = analogRead(SENSOR_PIN);
  float v_out = (raw_adc * VREF) / 1023.0;
  float i_meas = (v_out - ZERO_VOLTAGE) / SENSITIVITY;

  // Outer loop: Calculate error against target
  float error = TARGET_I - i_meas;

  // Discrete forward-Euler rectangular integral accumulation with anti-windup clamping
  integral_err += error * 0.005; // dt = 5 ms sample time (~200 Hz loop)
  integral_err = constrain(integral_err, -50.0, 50.0);

  // PI control effort synthesis
  float control_effort = (Kp * error) + (Ki * integral_err);

  // Modulate compare register OCR1A within hardware limits
  ocr_value = constrain(ocr_value + (int)control_effort, 50, TOP_PWM - 50);
  OCR1A = ocr_value;

  // Telemetry log output to Serial Monitor
  Serial.print("I_meas: "); Serial.print(i_meas, 2);
  Serial.print(" | Target: "); Serial.print(TARGET_I, 2);
  Serial.print(" | OCR1A: "); Serial.println(OCR1A);

  delay(5); // Loop sample rate ~ 200 Hz
}
