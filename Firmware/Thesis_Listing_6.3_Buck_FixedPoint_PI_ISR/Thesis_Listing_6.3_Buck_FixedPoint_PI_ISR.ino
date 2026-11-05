// =========================================================================
// Thesis Listing 6.3: Advanced Fixed-Point PI Controller using Timer1 ISR
// Reference: Graduation Project Thesis §6.2.5.2 (Listing 6.3)
// Source: graduation project team firmware
// Target: Buck Stage Current Regulation (BUCK_ICNTRL_PI, Io_ref = 0.3 A)
// Notes: 32-bit fixed-point scaling (SC_B=17), executed inside Timer 1 OVF ISR (10 kHz Fast PWM, ICR1 = 1599)
// =========================================================================

#include <Arduino.h>

#define SC_B 17 // number of bits for fixed point scaling
#define GEN_SCALE 131072L // 2^SC_B

#define dr_min (int32_t)(0.0f*GEN_SCALE) // minimum scaled duty cycle
#define dr_max (int32_t)(1.0f*GEN_SCALE) // maximum scaled duty cycle

#define I_SCALE (int32_t)(0.005f*GEN_SCALE) // scaled current sensor gain
#define OFFSET (int32_t)(22.643f*GEN_SCALE) // sensor offset 520/1024*5.12
#define sensitivity (int32_t)(0.185f*GEN_SCALE) // sensor sensitivity 185mV/A

#define Io_ref (int32_t)(0.3f*GEN_SCALE) // output current reference

#define B0_i (int32_t)(0.0094875*GEN_SCALE)
#define B1_i (int32_t)(-0.0076936*GEN_SCALE)
#define A1_i (int32_t)(1*GEN_SCALE)

#define MAX_DUTY 1599L // True maximum duty cycle

void PWM_TMR_INIT(); // PWM initialization function
void ADC_INIT(); // ADC initialization function
int32_t SAMPLE_IO(); // output current sampling function

int32_t GEN_CTRL_PI(int64_t m_r[], int32_t e_r[], int32_t ref_C, int32_t adc_C,
                    int32_t A1_g, int32_t B0_g, int32_t B1_g,
                    int32_t lo_limit, int32_t up_limit);

void BUCK_ICNTRL_PI(void); // PI current control

int64_t m_i[4] = {0}; // current control array
int32_t e_i[4] = {0}; // current error array

int32_t Io = 0; // output current

void setup() {
  PWM_TMR_INIT(); // Initialize the pwm
  ADC_INIT(); // Initialize the adc

  DDRB |= (1 << DDB5); // Initialize PORTB5 as a digital output
}

void loop() {
}

ISR(TIMER1_OVF_vect) {
  PORTB |= (1 << PORTB5); // set pin PORTB5

  BUCK_ICNTRL_PI(); // Execute PI current control
  PORTB &= ~(1 << PORTB5); // clear pin PORTB5
}

void PWM_TMR_INIT() {
  // Clear the TMR1 control registers
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;

  TCCR1A |= (1 << COM1A1); // Set up in non-inverting mode

  // Set up in fast pwm mode
  TCCR1A |= (1 << WGM11);
  TCCR1B |= (1 << WGM12);
  TCCR1B |= (1 << WGM13);

  // Set a prescale ratio of 1
  TCCR1B |= (1 << CS10);

  // PWM frequency of 10kHz
  ICR1 = 1599;

  // Set up pin D9 as an output
  pinMode(9, OUTPUT);

  // Set up tmr1 interrupt
  TIMSK1 |= (1 << TOIE1);
  sei();
}

void ADC_INIT() {
  // Set up adc current reference
  ADMUX &= ~(1 << REFS1);
  ADMUX |= (1 << REFS0);

  // adc clock prescale 16
  ADCSRA |= (1 << ADPS2);
  ADCSRA &= ~(1 << ADPS1);
  ADCSRA &= ~(1 << ADPS0);

  // enable the adc
  ADCSRA |= (1 << ADEN);
}

int32_t SAMPLE_IO() {
  // use channel ADC0 - pin A0
  ADMUX &= ~(1 << MUX3);
  ADMUX &= ~(1 << MUX2);
  ADMUX &= ~(1 << MUX1);
  ADMUX &= ~(1 << MUX0);

  ADCSRA |= (1 << ADSC); // set the ADSC bit
  while(ADCSRA & (1 << ADSC)); // poll the ADSC bit

  return ADC; // return the ADC conversion result
}

int32_t GEN_CTRL_PI(int64_t m_r[], int32_t e_r[], int32_t ref_C, int32_t adc_C,
                    int32_t A1_g, int32_t B0_g, int32_t B1_g,
                    int32_t lo_limit, int32_t up_limit) {

  m_r[0] = (A1_g*m_r[1] + B0_g*e_r[0] + B1_g*e_r[1]) >> SC_B; // compute the PI output control step

  // limit the control output
  if(m_r[0] < lo_limit) {
    m_r[0] = lo_limit;
  }
  else if(m_r[0] > up_limit) {
    m_r[0] = up_limit;
  }

  // return the control output
  return m_r[0];
}

void BUCK_ICNTRL_PI(void) {
  int32_t pwm;
  int32_t numerator = SAMPLE_IO() * I_SCALE - OFFSET;
  Io = ((numerator << SC_B) / sensitivity);

  e_i[0] = Io_ref - Io; // compute the output current error
  // compute the true pwm duty cycle using a PI controller
  pwm = (MAX_DUTY*GEN_CTRL_PI(m_i, e_i, Io_ref, Io,
                              A1_i, B0_i, B1_i,
                              dr_min, dr_max)) >> SC_B;
  m_i[1] = m_i[0]; // update the previous control output
  e_i[1] = e_i[0]; // update the previous error

  // write the pwm to the output capture register
  OCR1A = pwm;
}
