#include <Arduino.h>
#include "board_def.h"
#include "io.h"

#include <pico.h>
#include <hardware/structs/padsbank0.h>

void gpio_begin(void) {
  // Pinmode: INPUT,OUTPUT,INPUT_PULLUP,INPUT_PULLDOWN

  gpio_input(IO_UART_TX);
  gpio_input(IO_UART_RX);
  gpio_input(IO_AUX0);
  gpio_input(IO_AUX1);

  gpio_output(IO_LED1);
  gpio_output(IO_LED2);

  gpio_input(IO_CAM0);
  gpio_input(IO_CAM1);
  gpio_input(IO_CAM2);
  gpio_input(IO_CAM3);

  gpio_set(IO_FL_ON, 0);
  gpio_output(IO_FL_ON);

  gpio_set(IO_FAN_PWR_C, 0);
  gpio_output(IO_FAN_PWR_C);
  gpio_set(IO_FAN_PWM, 0);
  gpio_output(IO_FAN_PWM);
  gpio_input(IO_FAN_F);

  gpio_set(IO_LASER1, 0);
  gpio_output(IO_LASER1);
  gpio_set(IO_LASER2, 0);
  gpio_output(IO_LASER1);

  gpio_input(IO_S1_TRIG);
  gpio_input(IO_S2_TRIG);

  gpio_output(IO_LED_INT);

  // Internal pins
  gpio_input(IO_VBUS_PRESENT);
  //gpio_input(IO_ADC3_VSYS);  
}

void gpio_pin_mode(int gpio, int mode) {
  pinMode(gpio, mode);
}  

void gpio_set(int gpio, int state) {
  digitalWrite(gpio, state); 
}

bool gpio_get(int gpio) {
  return digitalRead(gpio); 
}

void set_laser(int n, bool mode) {
  if (n == 0 || n == 1)
    gpio_set(IO_LASER1 + n, mode);
}
