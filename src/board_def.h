#ifndef BOARD_DEF_H
#define BOARD_DEF_H

// MCU I/O pins
#define IO_UART_TX      0
#define IO_UART_RX      1
#define IO_AUX0         2
#define IO_AUX1         3
// #define IO_SDA       4
// #define IO_SCL       5
#define IO_CAM0         6
#define IO_CAM1         7
#define IO_CAM2         8
#define IO_CAM3         9
#define IO_LED1         10
#define IO_AUX2         10
#define IO_LED2         11
#define IO_AUX3         11
#define IO_FL_ON        12
#define IO_FAN_F        13
#define IO_FAN_PWM      14
#define IO_FAN_PWR_C    15
#define IO_LASER1       16
#define IO_LASER2       17
#define IO_S1_TRIG      18
#define IO_S2_TRIG      19
#define IO_S1_REF       20
#define IO_S2_REF       21
#define IO_LED_INT      25
#define IO_S1_AIN       26
#define IO_S2_AIN       27
#define IO_GND_AIN      28

#define ADC_CH_S1   0
#define ADC_CH_S2   1
#define ADC_CH_REF  2

// Internal RPi Pico connections
#define IO_DCDC_PS      23 // Power saving pin
#define IO_VBUS_PRESENT 24 // USB Vbus sensing pin
#define IO_ADC3_VSYS    29 // Vsys divided by 3

#endif
