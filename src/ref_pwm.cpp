#include "board_def.h"
#include "io.h"

#include "hardware/pwm.h"

static constexpr float vcc_volt = 3.3f; 

// PWM wrap value (defines the PWM period)
// Sets up 5mV step - 660 steps * 5mV = 3300mV 
#define PWM_STEPS 660
// On-board PWM develops minus 0.5-2% error in the middle of the range (around 1.65V)
// due to various factors of simplified analog design.
// gpio_set_drive_strength is 8mA for the measurements below.
// To compensate this error, we actually run PWM with a number of steps 
// equal to (PWM_STEPS-PWM_MISSING_CODES), but calculate values with the 
// full PWM_STEPS. This introduces positive correction to the values up 
// to the 1/2 of the scale. And we use only 1/2 of a PWM scale - we don't need 
// more for a ref output. This provides for a nice compensation over the 
// range of 0.0..1.65V. The voltage step remains 5V.
// Experimentally, it keeps error within +-2mV in [0V-1.6V] range.
#define PWM_MISSING_CODES 4
#define PWM_WRAP (PWM_STEPS - PWM_MISSING_CODES - 1)
static uint32_t ref_pwm_slice;
static uint32_t ref_pwm_setting[2] = {0};

static void set_ref_pwm(int chan, uint32_t setting) {
  if (setting > PWM_STEPS/2)
    setting = PWM_STEPS/2; // Produce 50% output max
    
  switch (chan) {
    case 0:     
      ref_pwm_setting[0] = setting;
      pwm_set_chan_level(ref_pwm_slice, PWM_CHAN_A, ref_pwm_setting[0]);
      break;
    case 1:     
      ref_pwm_setting[1] = setting;
      pwm_set_chan_level(ref_pwm_slice, PWM_CHAN_B, ref_pwm_setting[1]);
      break;
    default:
      break;
  }
}

static uint32_t get_ref_pwm(int chan) {
  if (chan != 0 && chan != 1)
    return 0;
  return ref_pwm_setting[chan];
}

void init_ref_pwm() {
    // Set GPIOs to the PWM function
    gpio_set_function(IO_S1_REF, GPIO_FUNC_PWM);
    gpio_set_function(IO_S2_REF, GPIO_FUNC_PWM);
    // Set 8mA drive on pin
    gpio_set_drive_strength(IO_S1_REF, GPIO_DRIVE_STRENGTH_8MA); 
    gpio_set_drive_strength(IO_S2_REF, GPIO_DRIVE_STRENGTH_8MA); 

    // Find out the PWM slice number (PWM2)
    // S1 and S2 shall be in the same slice
    ref_pwm_slice = pwm_gpio_to_slice_num(IO_S1_REF);
      
    // Get the PWM configuration 
    pwm_config config = pwm_get_default_config(); 
    // Set the divisor to 1 (no division, full speed) 
    pwm_config_set_clkdiv_int(&config, 1);
    // Set the PWM wrap value
    pwm_config_set_wrap(&config, PWM_WRAP);
    // Init channels 
    pwm_init(ref_pwm_slice, &config, true);

    set_ref_pwm(0, PWM_STEPS/2);
    set_ref_pwm(1, PWM_STEPS/2);

    // Enable the PWM slice
    pwm_set_enabled(ref_pwm_slice, true);
}


void set_ref_voltage(int ch, float V) {
    float pwm_step = vcc_volt / PWM_STEPS;
    set_ref_pwm(ch, round(V/pwm_step));
}

float get_ref_voltage(int ch) {
    float pwm_step = vcc_volt / PWM_STEPS;
    return get_ref_pwm(ch) * pwm_step;
}
