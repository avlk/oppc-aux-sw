#ifndef BOARD_IO_H
#define BOARD_IO_H

#include <Arduino.h>

void gpio_begin(void);

void gpio_pin_mode(int gpio, int mode);
#define gpio_input(gpio) gpio_pin_mode(gpio, INPUT)
#define gpio_input_pu(gpio) gpio_pin_mode(gpio, INPUT_PULLUP)
#define gpio_input_pd(gpio) gpio_pin_mode(gpio, INPUT_PULLDOWN)
#define gpio_output(gpio) gpio_pin_mode(gpio, OUTPUT)

void gpio_set(int gpio, int state);
bool gpio_get(int gpio);

void set_led(int led, bool state);

void set_laser(int n, bool mode);

// Sleeps given number of ms, FreeRTOS vTaskDelay under the hood
void task_sleep_ms(uint32_t ms);

#endif
