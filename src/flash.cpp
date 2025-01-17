#include <DS_MCP4018.h>
#include "flash.h"
#include "io.h"
#include "board_def.h"

// TODO: in this library the address of MCP40D18 is wrong and it won't work out of the box 
static MCP4018 FlashPowerPot;

void init_flash() {
    FlashPowerPot.begin();
    
    FlashPowerPot.reset();
    FlashPowerPot.setWiperPercent(100);
}


static uint8_t flash_level_to_wiper_setting(uint8_t percent) {
    // [0] -> 0V -> Flash OFF
    // [35..120] -> 0.9-3.1V -> CTRL 0.3-2.5V
    // [127] -> 3.3V -> Flash ON
    if (percent == 0)
        return 0;
    if (percent >= 100)
        return 127;
    
    float v = percent / 100.0f;
    return round(35.0 + 85.0 * v);
}


static uint8_t flash_wiper_setting_to_level(uint8_t wiper) {
    if (wiper < 35)
        return 0;
    if (wiper > 120)
        return 100;
    
    float v = ((float)wiper - 35) / 85.0f;
    return round(v * 100.0);
}

void set_flash_level(uint8_t percent) {
    FlashPowerPot.setWiperByte(flash_level_to_wiper_setting(percent));
}

uint8_t get_flash_level() {
    return flash_wiper_setting_to_level(FlashPowerPot.getWiperByte());
}

void set_flash(bool onoff) {
    gpio_set(IO_FL_ON, onoff);
}