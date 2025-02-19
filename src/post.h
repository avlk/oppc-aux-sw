#ifndef _POST_H
#define _POST_H

typedef struct {
    // Sensor output in dark condition
    float dark_v[2];
    // Sensor output when it's laser is lit (-dark_v)
    float ambient_single[2];
    // Sensor output when both lasers are lit (-dark_v)
    float ambient_both[2];
    // Sensor output with flash (-dark_v)
    float ambient_flash[2];
    // Test result for sensor trigger high level
    bool  trig_high[2];
    // Test result for sensor trigger low level
    bool  trig_low[2];
    // Setting for REF PWM
    float ref_level[2];
    // Overall test status
    bool pass;
} post_result_t;

bool post();

const post_result_t &get_post_result();
void print_post_result(const post_result_t &result);

#endif
