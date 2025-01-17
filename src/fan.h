#ifndef _FAN_H
#define _FAN_H

void init_fan();
void set_fan_speed(int duty_cycle);
int get_fan_speed();

void set_fan(bool onoff);

#endif
