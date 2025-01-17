#ifndef _REF_PWM_H
#define _REF_PWM_H

void init_ref_pwm();
void set_ref_voltage(int ch, float V);
float get_ref_voltage(int ch);

#endif
