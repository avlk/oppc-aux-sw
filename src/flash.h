#ifndef _FLASH_H
#define _FLASH_H

#include <stdint.h>

void init_flash();
void set_flash_level(uint8_t percent);
uint8_t get_flash_level();

void set_flash(bool onoff);

#endif
